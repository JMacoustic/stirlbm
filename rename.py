import os

start_old = 1
end_old = 460
start_new = 650

for i in range(start_old, end_old + 1):
    old_name = f"decay_{i:04d}.mp4"
    new_index = start_new + (i - start_old)
    new_name = f"decay_{new_index:04d}.mp4"

    if os.path.isdir(old_name):
        os.rename(old_name, new_name)


for i in range(start_old, end_old + 1):
    old_name = f"config_decay_{i:04d}.json"
    new_index = start_new + (i - start_old)
    new_name = f"config_decay_{new_index:04d}.json"

    if os.path.isdir(old_name):
        os.rename(old_name, new_name)