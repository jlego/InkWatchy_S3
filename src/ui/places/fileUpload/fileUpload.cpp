#include "fileUpload.h"

#if FILE_UPLOAD

#include <WiFi.h>
#include <esp_wifi.h>
#include <SD_MMC.h>
#include "littlefs.h"

static WiFiServer *uploadServer = nullptr;
static WiFiClient uploadClient;
static bool uploadRunning = false;
static String uploadStatus = "";
static const char *UPLOAD_DIR = "/upload_tmp";

static const char PAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>InkWatchy Upload</title>
<style>body{font-family:sans-serif;max-width:500px;margin:40px auto;padding:0 20px}
h1{font-size:1.3em}input[type=file]{width:100%;margin:10px 0}
button{background:#4CAF50;color:#fff;border:none;padding:10px 20px;cursor:pointer;border-radius:4px}
.file-list{margin-top:20px}.del{color:#c00;cursor:pointer;margin-left:8px}</style>
</head><body><h1>InkWatchy - File Upload</h1>
<form method="POST" action="/upload" enctype="multipart/form-data">
<input type="file" name="file" multiple><br>
<button type="submit">Upload</button></form>
<button onclick="if(confirm('Delete ALL books on SD card?'))fetch('/clean_sd').then(()=>loadFiles())" style="background:#c00">Clean SD /book/</button>
<div class="file-list" id="fl"></div>
<script>
function loadFiles(){fetch('/files').then(r=>r.text()).then(h=>{document.getElementById('fl').innerHTML=h})}
document.querySelector('form').onsubmit=function(){document.getElementById('fl').innerHTML='Uploading...';};
loadFiles();
function delFile(f){if(!confirm('Delete '+f+'?'))return;fetch('/delete?f='+encodeURIComponent(f)).then(()=>loadFiles())}
</script></body></html>
)rawliteral";

static void sendResponse(WiFiClient &client, int code, const char *type, const String &body)
{
    client.printf("HTTP/1.1 %d OK\r\n", code);
    client.printf("Content-Type: %s\r\n", type);
    client.printf("Content-Length: %d\r\n", body.length());
    client.print("Connection: close\r\n\r\n");
    client.print(body);
}

static String urlDecode(String str)
{
    String decoded = "";
    char c;
    for (unsigned int i = 0; i < str.length(); i++)
    {
        c = str.charAt(i);
        if (c == '%' && i + 2 < str.length())
        {
            char hex[3] = {str.charAt(i + 1), str.charAt(i + 2), 0};
            decoded += (char)strtol(hex, nullptr, 16);
            i += 2;
        }
        else if (c == '+')
        {
            decoded += ' ';
        }
        else
        {
            decoded += c;
        }
    }
    return decoded;
}

static void handleRequest(WiFiClient &client)
{
    String req = client.readStringUntil('\n');
    req.trim();
    debugLog("HTTP req: " + req);

    if (req.startsWith("GET / "))
    {
        String html = FPSTR(PAGE_HTML);
        html += "<br><b>SD card /book/:</b><br>";
        File sdList = LittleFS.open("/sd_file_list", FILE_READ);
        File mapList = LittleFS.open("/book_map", FILE_READ);
        if (sdList)
        {
            while (sdList.available())
            {
                String ln = sdList.readStringUntil('\n');
                ln.trim();
                if (ln.length() > 0)
                {
                    String showName = ln;
                    if (mapList)
                    {
                        mapList.seek(0);
                        while (mapList.available())
                        {
                            String ml = mapList.readStringUntil('\n');
                            ml.trim();
                            int eq = ml.indexOf('=');
                            if (eq > 0)
                            {
                                String mapSd = ml.substring(0, eq);
                                String mapDn = ml.substring(eq + 1);
                                String lnLow = ln;
                                String mapSdLow = mapSd;
                                for (unsigned int ci = 0; ci < lnLow.length(); ci++)
                                    if (lnLow[ci] >= 'A' && lnLow[ci] <= 'Z') lnLow[ci] += 32;
                                for (unsigned int ci = 0; ci < mapSdLow.length(); ci++)
                                    if (mapSdLow[ci] >= 'A' && mapSdLow[ci] <= 'Z') mapSdLow[ci] += 32;
                                if (mapSdLow == lnLow)
                                {
                                    showName = mapDn;
                                    break;
                                }
                            }
                        }
                    }
                    html += showName + " <span class='del' onclick=\"delFile('" + ln + "')\">[x]</span><br>";
                }
            }
            sdList.close();
        }
        if (mapList) mapList.close();
        File delList = LittleFS.open("/sd_delete_list", FILE_READ);
        if (delList)
        {
            html += "<br><b>Pending deletions:</b><br>";
            while (delList.available())
            {
                String ln = delList.readStringUntil('\n');
                ln.trim();
                if (ln.length() > 0) html += ln + "<br>";
            }
            delList.close();
        }
        sendResponse(client, 200, "text/html", html);
        return;
    }

    if (req.startsWith("GET /clean_sd"))
    {
        File flag = LittleFS.open("/sd_clean_all", FILE_WRITE);
        if (flag) { flag.println("1"); flag.close(); }
        LittleFS.remove("/sd_delete_list");
        debugLog("Clean SD /book/ requested");
        sendResponse(client, 200, "text/plain", "Will clean SD /book/ on exit");
        return;
    }

    if (req.startsWith("GET /files"))
    {
        String html = "<b>Upload queue (/upload_tmp/):</b><br>";
        File root = LittleFS.open(UPLOAD_DIR);
        if (root && root.isDirectory())
        {
            File f = root.openNextFile();
            while (f)
            {
                if (!f.isDirectory())
                {
                    String name = String(f.name());
                    int ls = name.lastIndexOf('/');
                    if (ls >= 0) name = name.substring(ls + 1);
                    html += name + " (" + String(f.size()) + " B)";
                    html += " <span class='del' onclick=\"delFile('" + name + "')\">[x]</span><br>";
                }
                f = root.openNextFile();
            }
        }
        html += "<br><b>SD card /book/ (delete on exit):</b><br>";
        File delList = LittleFS.open("/sd_delete_list", FILE_READ);
        if (delList)
        {
            while (delList.available())
            {
                String ln = delList.readStringUntil('\n');
                ln.trim();
                if (ln.length() > 0) html += ln + " <span class='del' onclick=\"delFile('" + ln + "')\">[pending del]</span><br>";
            }
            delList.close();
        }
        sendResponse(client, 200, "text/html", html);
        return;
    }

    if (req.startsWith("GET /delete?f="))
    {
        int paramStart = req.indexOf('?');
        int paramEnd = req.indexOf(' ', paramStart);
        String param = req.substring(paramStart + 3, paramEnd);
        String name = urlDecode(param);
        String littlefsPath = String(UPLOAD_DIR) + "/" + name;
        if (LittleFS.exists(littlefsPath))
        {
            LittleFS.remove(littlefsPath);
        }
        else
        {
            File delFile = LittleFS.open("/sd_delete_list", FILE_APPEND);
            if (delFile)
            {
                delFile.println(name);
                delFile.close();
            }
        }
        sendResponse(client, 200, "text/plain", "OK");
        return;
    }

    if (req.startsWith("POST /upload"))
    {
        String boundary = "";
        int contentLength = 0;
        while (client.available())
        {
            String header = client.readStringUntil('\n');
            header.trim();
            if (header.startsWith("Content-Type: multipart/form-data; boundary="))
            {
                boundary = header.substring(header.indexOf("boundary=") + 9);
            }
            if (header.startsWith("Content-Length:"))
            {
                contentLength = header.substring(15).toInt();
            }
            if (header.length() == 0) break;
            if (header == "\r") break;
        }

        if (boundary.length() == 0)
        {
            sendResponse(client, 400, "text/plain", "No boundary");
            return;
        }

        String dashBoundary = "--" + boundary;
        int uploadCount = 0;
        bool inData = false;
        File writeFile;
        String filename = "";
        int fileBytesRead = 0;
        unsigned long lastData = millis();

        while (client.connected() && (millis() - lastData < 10000))
        {
            if (!client.available())
            {
                delay(1);
                continue;
            }
            lastData = millis();
            String line = client.readStringUntil('\n');
            String trimmed = line;
            trimmed.trim();

            if (trimmed.startsWith("Content-Disposition:"))
            {
                int fnIdx = trimmed.indexOf("filename=\"");
                if (fnIdx >= 0)
                {
                    int fnStart = fnIdx + 10;
                    int fnEnd = trimmed.indexOf("\"", fnStart);
                    filename = trimmed.substring(fnStart, fnEnd);
                    debugLog("Upload filename: " + filename);
                }
            }
            else if (trimmed.startsWith("Content-Type:"))
            {
                continue;
            }
            else if (trimmed == dashBoundary || trimmed == dashBoundary + "--")
            {
                if (inData && writeFile)
                {
                    writeFile.close();
                    uploadCount++;
                    uploadStatus = "Done: " + filename;
                    debugLog("Uploaded: " + filename + " " + String(fileBytesRead) + " bytes");
                }
                inData = false;
                filename = "";
                fileBytesRead = 0;
                if (trimmed.endsWith("--")) break;
            }
            else if (trimmed.length() == 0 && filename != "" && !inData)
            {
                inData = true;
                String path = String(UPLOAD_DIR) + "/" + filename;
                LittleFS.remove(path);
                writeFile = LittleFS.open(path, FILE_WRITE);
                if (!writeFile)
                {
                    debugLog("Failed to open LittleFS for write: " + path);
                    inData = false;
                }
                fileBytesRead = 0;
                uploadStatus = "Uploading: " + filename;
                debugLog("Started writing: " + path);
            }
            else if (inData && writeFile)
            {
                writeFile.println(trimmed);
                fileBytesRead += trimmed.length() + 1;
            }
        }

        if (writeFile) writeFile.close();
        debugLog("Upload done, count: " + String(uploadCount));
        String msg = "Uploaded " + String(uploadCount) + " file(s)";
        sendResponse(client, 200, "text/plain", msg);
        return;
    }

    sendResponse(client, 404, "text/plain", "Not found");
}

void fileUploadInit()
{
    debugLog("fileUploadInit START");
    debugLog("Heap: " + String(ESP.getFreeHeap()));
    resetSleepDelay(120000);
    debugLog("fileUploadInit after resetSleepDelay");
    dis->fillScreen(SCWhite);
    setFont(&FreeSansBold9pt7b);
    setTextSize(1);
    dis->setTextColor(SCBlack);
    dis->setCursor(10, 30);
    dis->print("Reading SD card...");
    disUp(true);

    LittleFS.remove("/sd_file_list");
    if (sdCardSetup())
    {
        File sdList = LittleFS.open("/sd_file_list", FILE_WRITE);
        File sdRoot = SD_MMC.open("/book");
        if (sdRoot && sdRoot.isDirectory() && sdList)
        {
            File sf = sdRoot.openNextFile();
            while (sf)
            {
                if (!sf.isDirectory())
                {
                    String sn = String(sf.name());
                    int sls = sn.lastIndexOf('/');
                    if (sls >= 0) sn = sn.substring(sls + 1);
                    sdList.println(sn);
                }
                sf = sdRoot.openNextFile();
            }
            sdRoot.close();
        }
        if (sdList) sdList.close();
        sdCardEnd();
        debugLog("SD file list saved");
    }
    else
    {
        debugLog("SD card not available for file list");
    }

    dis->fillScreen(SCWhite);
    setFont(&FreeSansBold9pt7b);
    setTextSize(1);
    dis->setTextColor(SCBlack);
    dis->setCursor(10, 30);
    dis->print("WiFi AP starting...");
    disUp(true);

    debugLog("fileUpload: Starting WiFi AP, heap: " + String(ESP.getFreeHeap()));
    WiFi.persistent(false);
    WiFi.disconnect(true);
    delay(100);
    WiFi.mode(WIFI_AP);
    delay(100);
    bool apOk = WiFi.softAP("InkWatchy", "12345678", 1, false, 4);
    debugLog("fileUpload: softAP returned: " + String(apOk));
    delay(2000);
    IPAddress ip = WiFi.softAPIP();
    debugLog("fileUpload: WiFi AP IP: " + ip.toString());
    if (ip == IPAddress(0, 0, 0, 0))
    {
        debugLog("fileUpload: WiFi AP failed, restarting...");
        WiFi.softAPdisconnect(true);
        delay(500);
        WiFi.mode(WIFI_OFF);
        delay(500);
        WiFi.mode(WIFI_AP);
        delay(500);
        apOk = WiFi.softAP("InkWatchy", "12345678", 6);
        debugLog("fileUpload: softAP retry returned: " + String(apOk));
        delay(2000);
        ip = WiFi.softAPIP();
        debugLog("fileUpload: WiFi AP IP retry: " + ip.toString());
    }

    LittleFS.mkdir(UPLOAD_DIR);
    debugLog("fileUpload: Upload dir ready");

    uploadServer = new WiFiServer(80);
    uploadServer->begin();
    uploadRunning = true;

    dis->fillScreen(SCWhite);
    dis->setCursor(10, 20);
    dis->print("File Upload Ready");
    dis->setCursor(10, 45);
    setFont(&FreeSansBold9pt7b);
    dis->print("WiFi: InkWatchy");
    dis->setCursor(10, 62);
    dis->print("Pass: 12345678");
    dis->setCursor(10, 82);
    dis->print("http://");
    dis->setCursor(10, 99);
    dis->print(ip.toString());
    dis->setCursor(10, 115);
    dis->print("-> SD on exit");
    dis->setCursor(10, 135);
    setFont(&FreeSansBold9pt7b);
    dis->print("Press Back to exit");
    disUp(true);
    debugLog("fileUpload: Init complete");
}

void fileUploadLoop()
{
    if (!uploadRunning)
    {
        buttonState btn = useButton();
        if (btn != None)
        {
            setButton(Back);
        }
        return;
    }
    resetSleepDelay(120000);

    if (uploadServer)
    {
        WiFiClient newClient = uploadServer->available();
        if (newClient)
        {
            handleRequest(newClient);
            newClient.stop();
        }
    }

    if (uploadStatus != "")
    {
        String status = uploadStatus;
        uploadStatus = "";
        dis->fillRect(0, 135, 200, 20, SCWhite);
        setFont(&FreeSansBold9pt7b);
        dis->setCursor(10, 150);
        dis->print(status);
    }

    buttonState btn = useButton();
    if (btn == Back)
    {
        setButton(Back);
    }
}

void fileUploadExit()
{
    if (uploadServer)
    {
        uploadServer->stop();
        delete uploadServer;
        uploadServer = nullptr;
    }
    if (uploadClient)
    {
        uploadClient.stop();
    }
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    uploadRunning = false;
    uploadStatus = "";

    File flagFile = LittleFS.open("/upload_to_sd", FILE_WRITE);
    if (flagFile)
    {
        flagFile.println("1");
        flagFile.close();
    }
    debugLog("fileUpload: Rebooting to copy files to SD...");
    delay(500);
    ESP.restart();
}

#endif