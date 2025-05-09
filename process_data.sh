#!/bin/bash
set -e
eval "$(pyenv init -)"

# Get folder name
read -p "Enter folder name (default: noname): " customname
customname=${customname:-noname}  # Default to "noname" if empty

read -p "Do you want to rename the videos? (y/n): " do_rename
if [[ "$do_rename" == "y" || "$do_rename" == "Y" ]]; then
    read -p "Enter the last old index (end_old): " end_old
    read -p "Enter the new starting index (start_new): " start_new
fi

# Run your main process
python writemp4.py

# Create directories
mkdir -p "$customname/$customname"
mv export/videos "$customname/$customname"
mv export/parameters "$customname/$customname"

echo "moved file to new directory"

if [[ "$do_rename" == "y" || "$do_rename" == "Y" ]]; then
    # Renaming logic
    video_dir="$customname/$customname/videos"
    param_dir="$customname/$customname/parameters"
    sample_file=$(find "$video_dir" -maxdepth 1 -name "*.mp4" | head -n 1)

    if [[ -n "$sample_file" ]]; then
        prefix=$(basename "$sample_file" | sed -E 's/[0-9]{4}\.mp4$//')
    else
        echo "Error: No sample video file found to detect prefix."
        exit 1
    fi
    
    start_old=1

    for ((i=end_old; i>=start_old; i--)); do
        old_name=$(printf "$video_dir/${prefix}%04d.mp4" $i)
        new_index=$((start_new + i - start_old))
        new_name=$(printf "$video_dir/${prefix}%04d.mp4" $new_index)

        if [ -f "$old_name" ]; then
            mv "$old_name" "$new_name"
            echo "Renamed: $(basename "$old_name") → $(basename "$new_name")"
        fi
    done

    for ((i=start_new-1; i>=start_old; i--)); do
        del_name=$(printf "$video_dir/${prefix}%04d.mp4" $i)

        if [ -f "$del_name" ]; then
            rm "$del_name"
            echo "Deleted: $(basename "$del_name")"
        fi
    done

    for ((i=end_old; i>=start_old; i--)); do
        old_name=$(printf "$param_dir/config_${prefix}%04d.json" $i)
        new_index=$((start_new + i - start_old))
        new_name=$(printf "$param_dir/config_${prefix}%04d.json" $new_index)

        if [ -f "$old_name" ]; then
            mv "$old_name" "$new_name"
            echo "Renamed: $(basename "$old_name") → $(basename "$new_name")"
        fi
    done

    for ((i=start_new-1; i>=start_old; i--)); do
        del_name=$(printf "$param_dir/config_${prefix}%04d.json" $i)

        if [ -f "$del_name" ]; then
            rm "$del_name"
            echo "Deleted: $(basename "$del_name")"
        fi
    done
    echo "Renaming complete!"
fi
