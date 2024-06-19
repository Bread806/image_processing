import cv2

# 讀取圖片
image_path = "ninja.bmp"
gray_image = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)

# 確認圖片是否讀取成功
if gray_image is not None:
    # Canny 邊緣檢測 (30, 100)
    canny_edge_30_100 = cv2.Canny(gray_image, 30, 100)
    cv2.imwrite("res\\canny_30_100.png", canny_edge_30_100)

    # Canny 邊緣檢測 (10, 200)
    canny_edge_150_200 = cv2.Canny(gray_image, 150, 200)
    cv2.imwrite("res\\canny_100_150.png", canny_edge_150_200)

    print("Canny edge detection completed and images saved.")
else:
    print("Error: Image cannot be loaded!")
