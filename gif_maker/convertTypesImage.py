from PIL import Image
import sys

def change_type_img(img_path):
    Image.open(img_path).save(img_path.split('.')[0] + '.bmp')

def main():
    img_path = sys.argv[1]
    change_type_img(img_path)

if __name__ == "__main__":
    main()