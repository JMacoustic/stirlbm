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

video_dir="$customname/$customname/videos"
parma_dir="$customname/$customname/parameters"

echo "processing complete!"

if [[ "$do_rename" == "y" || "$do_rename" == "Y" ]]; then
    # Renaming logic
    start_old=1
    video_dir="$customname/$customname/videos"
    param_dir="$customname/$customname/parameters"

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
        old_name=$(printf "$param_dir/${prefix}%04d.json" $i)
        new_index=$((start_new + i - start_old))
        new_name=$(printf "$param_dir/${prefix}%04d.json" $new_index)

        if [ -f "$old_name" ]; then
            mv "$old_name" "$new_name"
            echo "Renamed: $(basename "$old_name") → $(basename "$new_name")"
        fi
    done

    for ((i=start_new-1; i>=start_old; i--)); do
        del_name=$(printf "$param_dir/${prefix}%04d.json" $i)

        if [ -f "$del_name" ]; then
            rm "$del_name"
            echo "Deleted: $(basename "$del_name")"
        fi
    done
    echo "Renaming complete!"
fi
