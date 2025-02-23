#!/bin/sh
#
# Script: generate_backup.sh
# Purpose: Read one or more file/directory paths (from a file list or a single argument),
#          copy them into a staging area preserving their relative paths,
#          remove unwanted subdirectories (specifically /overlay/root/root if backing up /overlay/root),
#          and then generate a SHA1 checksum file where the filenames are shown relative to the staging root.
#
# Usage:
#   ./generate_backup.sh file_list.txt
#     - Reads absolute paths (files or directories) from file_list.txt.
#
#   ./generate_backup.sh /some/path
#     - Backs up the specified path recursively.
#
#   ./generate_backup.sh
#     - Defaults to backing up /overlay/root.
#

# --- Determine Input Mode ---
found_overlay_root=0

if [ $# -lt 1 ]; then
    mode="single"
    single_path="/overlay/root"
elif [ -f "$1" ]; then
    mode="list"
    list_file="$1"
elif [ -d "$1" ]; then
    mode="single"
    single_path="$1"
else
    echo "Error: Provided argument is neither a file list nor a directory."
    exit 1
fi

# --- Directories and File Names ---
BACKUP_STAGING_DIR="/tmp/backup/staging"
FINAL_BACKUP_DIR="/tmp/backup"
TAR_FILE="$FINAL_BACKUP_DIR/backup.tar"
TAR_GZ_FILE="$FINAL_BACKUP_DIR/backup.tar.gz"
CHECKSUM_FILE="$BACKUP_STAGING_DIR/checksum.txt"

# --- Prepare Staging Area ---
rm -rf "$BACKUP_STAGING_DIR" 2>/dev/null
mkdir -p "$BACKUP_STAGING_DIR" || { echo "Error: Could not create staging directory '$BACKUP_STAGING_DIR'."; exit 1; }

# --- Function: process_path ---
# Copies a file or directory into the staging area preserving its relative path.
process_path() {
    filepath="$1"
    [ -z "$filepath" ] && return

    # Remove trailing slashes (unless the path is just "/")
    if [ "$filepath" != "/" ]; then
      filepath=$(echo "$filepath" | sed 's:/*$::')
    fi

    # Mark if the backup source is exactly /overlay/root
    if [ "$filepath" = "/overlay/root" ]; then
        found_overlay_root=1
    fi

    if [ -f "$filepath" ]; then
        REL_PATH=$(echo "$filepath" | sed 's|^/||')
        DEST_DIR="$BACKUP_STAGING_DIR/$(dirname "$REL_PATH")"
        mkdir -p "$DEST_DIR" || { echo "Error: Could not create subdirectory for '$REL_PATH' in staging."; exit 1; }
        cp "$filepath" "$DEST_DIR" || { echo "Error: Could not copy '$filepath' to staging area."; exit 1; }
    elif [ -d "$filepath" ]; then
        REL_PATH=$(echo "$filepath" | sed 's|^/||')
        DEST_PARENT="$BACKUP_STAGING_DIR/$(dirname "$REL_PATH")"
        mkdir -p "$DEST_PARENT" || { echo "Error: Could not create parent directory '$DEST_PARENT'."; exit 1; }
        cp -r "$filepath" "$DEST_PARENT" || { echo "Error: Could not copy directory '$filepath' to staging area."; exit 1; }
    else
        echo "Error: File or directory '$filepath' does not exist."
        exit 1
    fi
}

# --- Process Input ---
if [ "$mode" = "list" ]; then
    while IFS= read -r line; do
        process_path "$line"
    done < "$list_file"
elif [ "$mode" = "single" ]; then
    process_path "$single_path"
fi

# --- Remove Unwanted Subfolder ---
# If /overlay/root was among the sources, then in the staging area the files are under "overlay/root".
# Remove the extra "root" folder (i.e. /tmp/backup/staging/overlay/root/root) if it exists.
if [ $found_overlay_root -eq 1 ]; then
    unwanted_dir="$BACKUP_STAGING_DIR/overlay/root/root"
    if [ -d "$unwanted_dir" ]; then
        rm -rf "$unwanted_dir" || { echo "Error: Could not remove unwanted folder '$unwanted_dir' from staging area."; exit 1; }
    fi
fi

# --- Generate Checksum File with Relative Paths ---
# We walk the staging directory and, for each file, remove the staging prefix.
# (This sed substitution technique is a common method for converting absolute paths to relative ones :contentReference[oaicite:0]{index=0}.)
> "$CHECKSUM_FILE" || { echo "Error: Cannot write to checksum file '$CHECKSUM_FILE'."; exit 1; }
find "$BACKUP_STAGING_DIR" -type f ! -name "$(basename "$CHECKSUM_FILE")" | while IFS= read -r file; do
    # Remove the staging prefix to get the relative path.
    rel=$(echo "$file" | sed "s|^$BACKUP_STAGING_DIR/||")
    checksum=$(sha1sum "$file" | awk '{print $1}')
    echo "$checksum  $rel"
done > "$CHECKSUM_FILE" || { echo "Error: Failed to generate checksums."; exit 1; }

# --- Create Tar Archive and Compress ---
mkdir -p "$FINAL_BACKUP_DIR" || { echo "Error: Could not create final backup directory '$FINAL_BACKUP_DIR'."; exit 1; }
tar -C "$BACKUP_STAGING_DIR" -cvf "$TAR_FILE" . || { echo "Error: Failed to create tar file from staging directory."; exit 1; }
gzip -f "$TAR_FILE" || { echo "Error: Failed to compress tar file."; exit 1; }

echo "Backup archive successfully created at: $TAR_GZ_FILE"
exit 0
