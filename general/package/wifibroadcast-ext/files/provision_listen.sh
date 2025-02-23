#!/bin/sh
#
# Enhanced script that:
#  - Runs drone_provisioner in an infinite loop
#  - Traps Ctrl+C (SIGINT) and other signals
#  - Acts based on exit codes from drone_provisioner, including 5 for BACKUP
#  - Always runs cleanup steps on exit

# ----------------------------------------------------------
# Cleanup function (called on exit or signal)
# ----------------------------------------------------------
cleanup() {
  echo "[CLEANUP] Removing temporary directories..."
  rm -rf /tmp/flash
  rm -rf /tmp/bind
  rm -rf /tmp/backup
}

# ----------------------------------------------------------
# Trap signals for graceful shutdown
# ----------------------------------------------------------
trap "echo 'Received SIGINT (Ctrl+C). Exiting gracefully...'; exit 130" INT
trap "echo 'Received SIGTERM. Exiting gracefully...'; exit 143" TERM
# You can add more traps for other signals if desired:
# trap "echo 'Received SIGHUP. Exiting gracefully...'; exit 129" HUP

# This trap ensures cleanup is *always* done upon exit (any exit).
trap cleanup EXIT

# ----------------------------------------------------------
# Main loop
# ----------------------------------------------------------
while true; do
  
  # --------------------------------------------------------
  # Run drone_provisioner and capture its exit code
  # --------------------------------------------------------
  drone_provisioner --ip 0.0.0.0 --listen-duration 99999
  EXIT_CODE=$?

  echo "drone_provisioner exited with code $EXIT_CODE."

  # --------------------------------------------------------
  # Handle exit codes
  # --------------------------------------------------------
  case $EXIT_CODE in
    0)
      echo "Listen period ended. Exiting with code 0."
      exit 0
      ;;

    1)
      echo "Fatal errors. Exiting with code 1."
      exit 1
      ;;

    2)
      echo "File received and saved successfully (BIND). Continuing execution..."
      
      cd /tmp/bind || exit 2
      
      # Decompress the .tar.gz
      gunzip bind.tar.gz
      
      # Optional: validate that bind.tar now exists
      if [ ! -f bind.tar ]; then
          echo "ERR: bind.tar not found after gunzip."
          exit 2
      fi
      
      # Extract the tar
      tar x -f bind.tar
      
      # Detect the top-level directory name (assuming exactly one)
      extracted_dir="$(tar -tf bind.tar | head -n1 | cut -d/ -f1)"
      
      # Check that the directory exists
      if [ -n "$extracted_dir" ] && [ -d "$extracted_dir" ]; then
          cd "$extracted_dir" || exit 2
          echo "Changed directory to: $extracted_dir"
      else
          echo "ERR: Could not identify a single top-level directory from bind.tar"
          exit 2
      fi
      
      # Validate checksums
      if ! [ -f checksum.txt ] || ! sha1sum -c checksum.txt
      then
          echo "ERR: Checksum failed."
          exit 2
      fi

      # -----------------------------------------------------
      # Copy system files, as needed
      # -----------------------------------------------------
      if [ -d overlay/ ]; then
          cp -r overlay/root/* /
          echo "Overlay files copied to root."
      fi
      
      if [ -f etc/wfb.yaml ]; then
          cp etc/wfb.yaml /etc/wfb.yaml
          echo "Copy success: /etc/wfb.yaml"
      fi

      if [ -d etc/sensors/ ]; then
          cp etc/sensors/* /etc/sensors/
          echo "Copy success: Sensor bins"
      fi

      if [ -f etc/majestic.yaml ]; then
          cp etc/majestic.yaml /etc/majestic.yaml
          /etc/init.d/S95majestic restart
          echo "Copy & restart success: /etc/majestic.yaml"
      fi

      if [ -f lib/modules/4.9.84/sigmastar/sensor_imx335_mipi.ko ]; then
          cp lib/modules/4.9.84/sigmastar/sensor_imx335_mipi.ko \
             /lib/modules/4.9.84/sigmastar/sensor_imx335_mipi.ko
          echo "Copy success (restart required): sensor_imx335_mipi.ko"
      fi
      
      #Set various settings
      wifi_profile_name=$(yaml-cli -i /etc/wfb.yaml -g .wireless.wifi_module)
      fw_setenv wifi_profile ${wifi_profile_name:-default}
      
      vtx_name=$(yaml-cli -i /etc/wfb.yaml -g .wireless.vtx_name)
      fw_setenv vtx_name ${vtx_name:-default}
             
      #Usage: /usr/bin/set_bitrate.sh <target_bitrate_in_kbps> [max_mcs] [--cap <cap_value>] [--max_bw <20|40>]
      set_bitrate.sh $(yaml-cli -i /etc/majestic.yaml -g .video0.bitrate) 5 --max_bw $(yaml-cli -i /etc/wfb.yaml -g .wireless.max_bw)
      
      #if passphrase is set, generate /etc/drone.key
      pass="$(yaml-cli -i /etc/wfb.conf -g .wireless.passhrase)" && [ -n "$pass" ] && keygen "$pass"

      if [ -f ./custom_script.sh ]; then
          chmod +x ./custom_script.sh
          ./custom_script.sh
          echo "Copy success and executed: custom_script.sh"
      fi

      # Cleanup BIND dir before continuing the loop
      rm -rf /tmp/bind
      
      continue
      ;;

    3)
      echo "UNBIND command received. Executing firstboot..."
      firstboot
      exit 3
      ;;

    4)
      echo "FLASH command received. Exiting with code 4."
      # (Insert your FLASH code here, currently not implemented)
      exit 4
      ;;

    5)
      echo "Backup completed. Continuing execution..."
      continue
      ;;

    *)
      echo "Unexpected error occurred. Exiting with code $EXIT_CODE."
      exit "$EXIT_CODE"
      ;;
  esac
  
done
