import cv2
import numpy as np

# Load your floor texture
floor = cv2.imread('skybox/floor.jpg')
face_size = floor.shape[0]  # assuming square faces

# Create white textures for other faces
white_face = np.ones_like(floor) * 255

# Cubemap layout: +X, -X, +Y, -Y, +Z, -Z
# Assuming +Z is front, +Y is up, and -Y is floor
faces = {
    'right': white_face,
    'left': white_face,
    'top': white_face,
    'bottom': floor,
    'front': white_face,
    'back': white_face
}

# Create equirectangular panorama
def cubemap_to_equirect(faces, out_h=512, out_w=1024):
    output = np.zeros((out_h, out_w, 3), dtype=np.uint8)

    for y in range(out_h):
        for x in range(out_w):
            theta = (x / out_w) * 2 * np.pi - np.pi
            phi = (1 - y / out_h) * np.pi - np.pi / 2

            vx = np.cos(phi) * np.sin(theta)
            vy = np.sin(phi)
            vz = np.cos(phi) * np.cos(theta)

            abs_v = np.abs([vx, vy, vz])
            max_axis = np.argmax(abs_v)
            major = [vx, vy, vz][max_axis]
            uc = vc = 0
            face = ''

            if max_axis == 0:  # x
                face = 'right' if vx > 0 else 'left'
                uc = -vz / abs(vx)
                vc = -vy / abs(vx)
            elif max_axis == 1:  # y
                face = 'top' if vy > 0 else 'bottom'
                uc = vx / abs(vy)
                vc = vz / abs(vy)
            else:  # z
                face = 'front' if vz > 0 else 'back'
                uc = vx / abs(vz)
                vc = -vy / abs(vz)

            u = int(((uc + 1) / 2) * (face_size - 1))
            v = int(((vc + 1) / 2) * (face_size - 1))

            color = faces[face][v, u]
            output[y, x] = color

    return output

# Convert and save
equirect = cubemap_to_equirect(faces)
cv2.imwrite('skybox/skybox.png', equirect)
print("image transform done!")