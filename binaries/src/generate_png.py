import sys
from load_values import load_values
from PIL import Image, warnings
warnings.simplefilter('ignore', Image.DecompressionBombWarning)


def generate_png(input_filename, output_filename):
    img_array = load_values(input_filename)
    xSize = len(img_array[0])
    ySize = len(img_array)
    defaultCol = (255, 0, 0)

    img = Image.new('RGBA', (xSize, ySize), defaultCol)
    img_data = img.load()

    for y in range(0, ySize):
        for x in range(0, xSize):
            img_data[x, y] = img_array[y][x]

    img.save(output_filename)


if __name__ == "__main__":
    if len(sys.argv) > 2:
        generate_png(sys.argv[1], sys.argv[2])
    else:
        generate_png("okay.pd", "output.png")