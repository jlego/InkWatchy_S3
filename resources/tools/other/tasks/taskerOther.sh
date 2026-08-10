#!/bin/bash

source resources/tools/globalFunctions.sh

OPTIONS=("1" "Reset choosed serial port" 
         "2" "Create demo image"
         "3" "Launch watchy-scom"
         "4" "Compile, Upload, Scom"
         "5" "Get calendar events and upload them")

NUM_OPTIONS=$((${#OPTIONS[@]} / 2))
HEIGHT=$((NUM_OPTIONS + 7))
WIDTH=60
CHOICE_HEIGHT=$((NUM_OPTIONS + 1))

TITLE="Other options"
MENU="Choose:"

if command -v dialog >/dev/null 2>&1; then
    CHOICE=$(dialog --clear \
                    --backtitle "$TITLE" \
                    --title "$TITLE" \
                    --menu "$MENU" \
                    $HEIGHT $WIDTH $CHOICE_HEIGHT \
                    "${OPTIONS[@]}" \
                    2>&1 >/dev/tty)
    clear
else
    echo "$TITLE"
    echo "$MENU"
    local i=0
    while [[ $i -lt ${#OPTIONS[@]} ]]; do
        echo "  ${OPTIONS[$i]}. ${OPTIONS[$i+1]}"
        i=$((i + 2))
    done
    read -p "Enter your choice: " CHOICE
fi

# cd resources/tools/other/wifiTool/ && ./patch.sh
case $CHOICE in
    1)
        rm -rf /tmp/inkwatchy_port
        ;;
    2)
        if command -v dialog >/dev/null 2>&1; then
            dialog --yesno "Do you want to clean and regenerate config?" 7 50
            cleanConfig=$?
        else
            read -p "Do you want to clean and regenerate config? (y/N): " response
            if [[ "$response" =~ ^[Yy]$ ]]; then
                cleanConfig=0
            else
                cleanConfig=1
            fi
        fi

        envList=("Watchy_1" "Watchy_1_5" "Watchy_2" "Watchy_3" "Yatchy" "All")
        
        if command -v dialog >/dev/null 2>&1; then
            envChoice=$(dialog --clear \
                        --title "Select environment" \
                        --menu "Environment:" 15 50 6 \
                        $(for i in "${!envList[@]}"; do echo "$i ${envList[$i]}"; done) \
                        2>&1 >/dev/tty)
            clear
        else
            echo "Select environment:"
            for i in "${!envList[@]}"; do
                echo "  $i. ${envList[$i]}"
            done
            read -p "Enter the number: " envChoice
        fi

        selectedEnv="${envList[$envChoice]}"

        if [[ "$selectedEnv" == "All" ]]; then
            if [[ "$cleanConfig" == 1 ]]; then
                echo "you have to clean config if you want all"
                exit
            fi
            resources/tools/other/compile/demoMaker.sh "" "$cleanConfig"
        else
            if [[ "$cleanConfig" == 0 ]]; then
                if [[ "$selectedEnv" == "Yatchy" ]]; then
                    if command -v dialog >/dev/null 2>&1; then
                        accChoice=$(dialog --clear \
                                    --title "Accelerometer" \
                                    --menu "Select accelerometer option:" 15 50 4 \
                                    1 "Use BMA456" \
                                    2 "Use BMA530" \
                                    3 "Disable accelerometer" \
                                    2>&1 >/dev/tty)
                        clear
                    else
                        echo "Select accelerometer option:"
                        echo "  1. Use BMA456"
                        echo "  2. Use BMA530"
                        echo "  3. Disable accelerometer"
                        read -p "Enter the number: " accChoice
                    fi
                else
                    if command -v dialog >/dev/null 2>&1; then
                        dialog --yesno "Do you want to disable the accelerometer?" 7 50
                        if [[ $? == 0 ]]; then
                            accChoice=3
                        else
                            accChoice=0
                        fi
                    else
                        read -p "Do you want to disable the accelerometer? (y/N): " response
                        if [[ "$response" =~ ^[Yy]$ ]]; then
                            accChoice=3
                        else
                            accChoice=0
                        fi
                    fi
                fi
            else
                accChoice=""
            fi
            resources/tools/other/compile/demoMaker.sh "$selectedEnv" "$cleanConfig" "$accChoice"
        fi
        ;;
    3)
        baudrate=$(extract_monitor_speed platformio.ini)
        serial_port=$(extract_serial_port "resources/tools/other/in/esptool")
        if [[ $? -ne 0 || -z "$serial_port" ]]; then
            echo "Failed to detect a valid serial port" >&2
            exit 1
        fi
        cd src/other/scomTask/watchy-scom/
        ./generate_symlinks.sh
        cd watchy-scom/
        cargo run --release -- -b $baudrate -p $serial_port
        ;;
    4)
        baudrate=$(extract_monitor_speed platformio.ini)
        serial_port=$(extract_serial_port "resources/tools/other/in/esptool")
        if [[ $? -ne 0 || -z "$serial_port" ]]; then
            echo "Failed to detect a valid serial port" >&2
            exit 1
        fi
        pio_env=$(get_pio_env .vscode/launch.json)
        ~/.platformio/penv/bin/pio run -e $pio_env -t upload --upload-port $serial_port
        cd src/other/scomTask/watchy-scom/
        ./generate_symlinks.sh
        cd watchy-scom/
        cargo run --release -- -b $baudrate -p $serial_port
        ;;
    5)
        resources/tools/other/calendar/getEvents.sh
        ;;
esac
