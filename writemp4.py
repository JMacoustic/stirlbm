import cv2
import os
import glob

export_dir = "export"
data_folders = sorted([f for f in os.listdir(export_dir) if (f.startswith("video_") or f.startswith("decay_") or f.startswith("steady_"))])
fps = 10 
output_format = "mp4v"

for folder in data_folders:
    folder_path = os.path.join(export_dir, folder)

    image_files = sorted(glob.glob(os.path.join(folder_path, "image_*.png")))
    
    if not image_files:
        print(f"No images found in {folder}, skipping...")
        continue

    first_frame = cv2.imread(image_files[0])
    #first_frame = cv2.cvtColor(first_frame, cv2.COLOR_BGR2RGB)
    height, width, layers = first_frame.shape

    output_video_path = os.path.join("export/videos", f"{folder}.mp4")

    fourcc = cv2.VideoWriter_fourcc(*output_format)
    video_writer = cv2.VideoWriter(output_video_path, fourcc, fps, (width, height))

    print(f"Processing {folder} ({len(image_files)} frames)...")

    for img_path in image_files:
        frame = cv2.imread(img_path)
        # frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        if frame is None:
            print(f"Skipping corrupted image: {img_path}")
            continue
        video_writer.write(frame)

    video_writer.release()
    print(f"Saved video: {output_video_path}")

print("All videos are processed.")