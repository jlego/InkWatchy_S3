xxd_wrapper(){
    all_args="$@"

    has_n=false
    next_arg=""

    while [ "$#" -gt 0 ]; do
        case "$1" in
            -n)
                has_n=true
                next_arg="$2"
                shift 2
                ;;
        esac
            shift
    done

    # Remove -n and its following argument
    all_args="${all_args//-n $next_arg}"

    # We can't save stdin inside a variable because of null bytes - learned it the hard way.
    converted_data=$(cat | xxd $all_args)

    # echo "All arguments: \"$all_args\"" >&2
    # echo "Has -n: $has_n" >&2
    if [ "$has_n" = true ]; then
        # echo "Next argument after -n: $next_arg" >&2
        echo -e -n "const unsigned char $next_arg[] = {\n" 
        echo "$converted_data"
        echo -e "};\nconst size_t ${next_arg}Length = (sizeof($next_arg)/sizeof($next_arg[0]));"
    else
        echo "$converted_data"
    fi
}

get_pio_env() {
    local json_file=$1
    local inkwatchy_env_file="/tmp/inkwatchy_env"

    if [ -f "$inkwatchy_env_file" ]; then
        env_name=$(tr -d '\n\r ' < "$inkwatchy_env_file")
        echo "Env name found in inkwatchy env file in /tmp/: $env_name" >&2
        echo "$env_name"
        return 
    fi
    echo "File $inkwatchy_env_file not found. Trying to use the json file" >&2
    if [ -f "$json_file" ]; then
        # Use jq to extract the projectEnvName from launch.json
        local env_name=$(grep -v '^ *//' "$json_file" | jq '.configurations[0].projectEnvName')
        env_name=${env_name//\"/}
        
        if [ -n "$env_name" ]; then
            if [ "$env_name" != "null" ]; then
                echo "Env name found in $json_file json file: $env_name" >&2
                echo "$env_name"
                return
            else 
                echo "The json answer is a null" >&2
            fi
        else
            echo "projectEnvName not found in any configuration in $json_file" >&2
        fi
    fi
    echo "File $json_file not found or not valid. Trying to use env variable" >&2
    if [ -n "$PLATFORMIO_ENV_NAME" ]; then
        echo "PLATFORMIO_ENV_NAME is set with value: $PLATFORMIO_ENV_NAME" >&2
        echo $PLATFORMIO_ENV_NAME
        return
    fi
    echo "PLATFORMIO_ENV_NAME not set, trying to read platformio.ini" >&2
    ini_path="${json_file%%.vscode/*}platformio.ini"
    if [ -f $ini_path ]; then
        default_env=$(grep 'default_envs' "$ini_path" | awk '{print $3}')
        echo "default env found: $default_env" >&2
        echo $default_env
        return
    fi
    echo "Failed to read platformio.ini at $ini_path, trying to get the directory?"
    pio_path="${json_file%%.vscode/*}.pio/"
    first_folder=$(ls ${pio_path}libdeps/ | head -n 1)
    echo "first_folder found: $first_folder" >&2
    echo $first_folder
    return
}

# Old
# output=$("$esptool_path" flash_id 2>&1)
# last_port=$(echo "$output" | grep "Serial port" | awk '{print $3}' | tail -n 1)
# echo -n "$last_port"
find_esptool() {
    if command -v esptool >/dev/null 2>&1; then
        echo "esptool"
        return
    fi
    if command -v esptool.py >/dev/null 2>&1; then
        echo "esptool.py"
        return
    fi
    if [[ -f ~/.platformio/penv/bin/esptool ]]; then
        echo "~/.platformio/penv/bin/esptool"
        return
    fi
    if [[ -f ~/.platformio/tools/tool-esptoolpy/esptool ]]; then
        echo "~/.platformio/tools/tool-esptoolpy/esptool"
        return
    fi
    if [[ -f ~/.espressif/python_env/idf*/bin/esptool.py ]]; then
        ls ~/.espressif/python_env/idf*/bin/esptool.py 2>/dev/null | head -n 1
        return
    fi
    echo ""
}

extract_serial_port() {
    local esptool_path=$1
    local port
    local current_day
    current_day=$(date +%d)

    local esptool_cmd=$(find_esptool)
    if [[ -z "$esptool_cmd" ]]; then
        echo "No esptool found in system. Using provided path: $esptool_path" >&2
        esptool_cmd="$esptool_path"
    else
        echo "Found esptool: $esptool_cmd" >&2
    fi

    if [[ -f /tmp/inkwatchy_port_day ]]; then
        local saved_day
        saved_day=$(tr -d '\n' < /tmp/inkwatchy_port_day)
        if [[ "$current_day" != "$saved_day" ]]; then
            echo "Stale port day cache detected ($saved_day vs today $current_day). Clearing cache." >&2
            rm -f /tmp/inkwatchy_port /tmp/inkwatchy_port_day
        fi
    fi

    if [[ -f /tmp/inkwatchy_port ]]; then
        port=$(tr -d '\n' < /tmp/inkwatchy_port)
        echo "Found cached port: $port. Testing validity..." >&2
        if "$esptool_cmd" --port "$port" chip_id >&2; then
            echo "Cached port $port is valid. Choosing this port." >&2
            echo -n "$port"
            return
        else
            echo "Cached port $port failed validity check. Clearing cache." >&2
            rm -f /tmp/inkwatchy_port /tmp/inkwatchy_port_day
        fi
    fi

    local ports=()
    if [[ "$(uname)" == "Darwin" ]]; then
        ports+=(/dev/cu.usbmodem* /dev/tty.usbmodem* /dev/cu.usbserial* /dev/tty.usbserial*)
    else
        ports+=(/dev/ttyUSB* /dev/ttyACM*)
    fi

    local choices=()
    local port_index=0
    for p in "${ports[@]}"; do
        if [[ -e $p ]]; then
            local desc=""
            if [[ "$(uname)" == "Darwin" ]]; then
                desc=$(ioreg -p IOUSB -c IOUSBDevice -k IOUSBSerialNumber -r 2>/dev/null | grep -A 5 "$(basename "$p")" | grep "USB Product Name" | awk -F'"' '{print $4}' | head -n 1)
            elif command -v udevadm >/dev/null 2>&1; then
                desc=$(udevadm info --query=property --name="$p" 2>/dev/null | grep -E '^ID_MODEL_FROM_DATABASE=|^ID_MODEL=' | head -n 1 | cut -d= -f2)
            fi
            if [[ -z "$desc" && -f "/sys/class/tty/${p##*/}/device/../interface" ]]; then
                desc=$(cat "/sys/class/tty/${p##*/}/device/../interface" 2>/dev/null)
            fi
            if [[ -z "$desc" && -f "/sys/class/tty/${p##*/}/device/../product" ]]; then
                desc=$(cat "/sys/class/tty/${p##*/}/device/../product" 2>/dev/null)
            fi
            [[ -z "$desc" ]] && desc="Unknown USB Serial Device"
            choices+=("$port_index" "$p - $desc")
            port_index=$((port_index + 1))
        fi
    done

    if [[ ${#choices[@]} -eq 0 ]]; then
        echo "No serial ports found. Please connect your device." >&2
        exit 1
    fi

    if command -v dialog >/dev/null 2>&1; then
        local cmd=(dialog --clear --menu "Select serial port" 15 50 5)
        local selection=$("${cmd[@]}" "${choices[@]}" 2>&1 >/dev/tty)
        if [[ -z "$selection" ]]; then
            echo "No port selected." >&2
            exit 1
        fi
        port=$(echo "${choices[@]}" | tr ' ' '\n' | sed -n "$((selection * 2 + 2))p")
    else
        echo "Available serial ports:" >&2
        local i=0
        while [[ $i -lt ${#choices[@]} ]]; do
            echo "  ${choices[$i]}. ${choices[$i+1]}" >&2
            i=$((i + 2))
        done
        read -p "Enter the number of the port to use: " selection >&2
        port=$(echo "${choices[@]}" | tr ' ' '\n' | sed -n "$((selection * 2 + 2))p")
        if [[ -z "$port" ]]; then
            echo "Invalid selection." >&2
            exit 1
        fi
        port=$(echo "$port" | awk '{print $1}')
    fi

    echo "User selected port: $port" >&2
    echo "Checking the selected port if it's valid" >&2
    if "$esptool_cmd" --port "$port" chip_id >&2; then
        echo "Selected port $port is valid. Saving to cache and choosing this port." >&2
        echo -n "$port" > /tmp/inkwatchy_port
        echo -n "$current_day" > /tmp/inkwatchy_port_day
        echo -n "$port"
    else
        echo "Selected port failed esptool check" >&2
        rm -f /tmp/inkwatchy_port /tmp/inkwatchy_port_day
        exit 1
    fi
}

extract_monitor_speed() {
    local file_path="$1"
    if [[ -f "$file_path" ]]; then
        local result=$(grep 'monitor_speed' "$file_path" | awk '{print $3}')
        echo -n "$result"
    else
        echo "File not found: $file_path"
    fi
}

check_define() {
    local define="$1"
    local file_path="$2"

    if [[ ! -f "$file_path" ]]; then
        echo "File not found: $file_path"
        return 0
    fi

    # Check if the define is turned on
    if grep -q "#define $define 1" "$file_path"; then
        return 1  # Define is turned on
    else
        return 0  # Define is not turned on
    fi
}
