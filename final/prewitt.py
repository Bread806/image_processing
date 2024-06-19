import cv2
import numpy as np

# 讀取圖片
image_path = "ninja.bmp"  # 替換成您的圖像路徑
gray_image = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)

# 確認圖片是否讀取成功
if gray_image is not None:
    # 定義 Prewitt 核
    kernelx = np.array([[1, 0, -1], [1, 0, -1], [1, 0, -1]], dtype=int)
    kernely = np.array([[1, 1, 1], [0, 0, 0], [-1, -1, -1]], dtype=int)

    # 應用 Prewitt 邊緣檢測
    prewittx = cv2.filter2D(gray_image, -1, kernelx)
    prewitty = cv2.filter2D(gray_image, -1, kernely)

    # 合併結果
    prewitt = cv2.addWeighted(prewittx, 0.5, prewitty, 0.5, 0)
    
    # 保存結果圖片
    output_path_prewitt = "res\\prewitt_out_ninja.png"
    cv2.imwrite(output_path_prewitt, prewitt)
    print(f"Prewitt edge detection completed. Result saved as {output_path_prewitt}")
else:
    print("Error: Image cannot be loaded!")
