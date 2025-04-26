# Renaming logic (reverse order to avoid overwrite)
start_old=1
for ((i=end_old; i>=start_old; i--)); do
    old_name=$(printf "decay_%04d.mp4" $i)
    new_index=$((start_new + i - start_old))
    new_name=$(printf "decay_%04d.mp4" $new_index)

    if [ -f "$old_name" ]; then
        mv "$old_name" "$new_name"
        echo "Renamed: $old_name → $new_name"
    fi
done

echo "Renaming complete!"