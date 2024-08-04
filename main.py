from PIL import Image
from apiclient.discovery import build
from random import randint
import numpy as np
import pyautogui
import time
import json
import requests
from io import BytesIO
from dotenv import load_dotenv
import os

# Import ScreenCapture class from screen_capture.py
from screen_capture import ScreenCapture

# Load environment variables from .env file
load_dotenv()
# Retrieve the Google Custom Search API key from environment variable
GOOGLE_CUSTOM_SEARCH_DEVELOPER_KEY = os.getenv('GOOGLE_CUSTOM_SEARCH_DEVELOPER_KEY')
pyautogui.PAUSE = 0.00001

            

def select_area():
    capture = ScreenCapture()
    capture.listener.join()
    return [capture.start_x, capture.start_y]


def scale_image(input_image_path, size):
    with Image.open(input_image_path) as image:
        scaled_image = image.resize(size, Image.LANCZOS)
        print(f"Image scaled to {size}")
        return scaled_image


def convert_to_custom_colors(image, custom_colors):
    image = image.convert("RGB")
    image_array = np.array(image)
    pixels = image_array.reshape(-1, 3)
    
    def closest_color(pixel, custom_colors):
        distances = np.sqrt(np.sum((custom_colors - pixel) ** 2, axis=1))
        return custom_colors[np.argmin(distances)]
    
    custom_colors = np.array(custom_colors)
    new_pixels = np.array([closest_color(pixel, custom_colors) for pixel in pixels])
    new_image_array = new_pixels.reshape(image_array.shape)
    new_image = Image.fromarray(new_image_array.astype('uint8'), 'RGB')
    
    return new_image


def select_color_from_palette(palette, color, timeout=5, bias=12):
    color_palette = pyautogui.locateOnScreen(palette["name"], minSearchTime=timeout)

    for cell in palette["data"]:
        if cell['r'] == color[0] and cell['g'] == color[1] and cell['b'] == color[2]:
            # match found in json
            pyautogui.click(x=color_palette.left+bias+(palette["cellW"]*cell['j']), y=color_palette.top+bias+(palette["cellH"]*cell['i']))


def get_custom_colors_array(path):
     with open(path) as f:
        palette_data = json.load(f)
        custom_colors = [[cell["r"], cell["g"], cell["b"]] for cell in palette_data["data"]]
        return custom_colors


def init(color_palette, image, current_color, start_x, start_y, brush_size=4, bias=12):
    
    pixels = image.load()
    width, height = image.size
    select_color_from_palette(color_palette, current_color, bias=bias)

    for y in range(height):
        counter = 0
        for x in range(width):
            pixel_value = pixels[x, y]
            if pixel_value != (255,255,255):
                counter = counter + 1
                if counter == brush_size:
                    pyautogui.click(x=start_x+x, y=start_y+y)
                    counter = 0


def download_image(search_term, output_path):
    try:
        service = build("customsearch", "v1", developerKey=GOOGLE_CUSTOM_SEARCH_DEVELOPER_KEY)
        res = service.cse().list(
            q=f"{search_term} white background",
            cx='7204b6b1decb42058',
            searchType='image',
            imgSize="LARGE",
            safe='high'
            ).execute()

        response = requests.get(res['items'][0]['link'])
        response.raise_for_status()

        image = Image.open(BytesIO(response.content))

        image.save(output_path)
        print(f"Image successfully saved to {output_path}")

    except requests.exceptions.RequestException as e:
        print(f"Error downloading the image: {e}")
    except IOError as e:
        print(f"Error saving the image: {e}")

def split_colors(color_palette, image, start_x, start_y, brush_size, bias):
    image_array = np.array(image)
    
    unique_colors, counts = np.unique(image_array.reshape(-1, image_array.shape[2]), axis=0, return_counts=True)
    
    sorted_indices = np.argsort(counts)
    sorted_colors = unique_colors[sorted_indices]
    
    if image_array.shape[2] == 4:
        black_pixel = [0, 0, 0, 255]
    else:
        black_pixel = [0, 0, 0]
        
    black_index = np.where((sorted_colors == black_pixel).all(axis=1))[0]
    
    if black_index.size > 0:
        black_index = black_index[0]
        sorted_colors[[0, black_index]] = sorted_colors[[black_index, 0]]
    
    for i, color in enumerate(sorted_colors):
        mask = np.all(image_array == color, axis=-1)
        new_image_array = np.ones_like(image_array) * 255
        new_image_array[mask] = color
        new_image = Image.fromarray(new_image_array.astype(np.uint8))
        
        init(color_palette, new_image, color, start_x, start_y, brush_size=brush_size, bias=bias)



if __name__ == '__main__':

    # best configuration
    INPUT_PATH = 'input.png'
    PALETTE_PATH = 'palette/palette.json'
    BRUSH_SIZE = 4
    BIAS = 12
    CANVAS_SIZE = (500,352)
    custom_colors = get_custom_colors_array(PALETTE_PATH)
    
    try:
        color_palette = json.load(open(PALETTE_PATH))
        search_term = pyautogui.prompt(text="Enter an image search term", default='YES', title='Skribbl IO Bot')
        download_image(search_term, INPUT_PATH)
        INPUT_SCALED = scale_image(INPUT_PATH, CANVAS_SIZE)
        # convert into an 8-bit colored image
        print('Converting Image...')
        converted_image = convert_to_custom_colors(INPUT_SCALED, custom_colors)
        print('Select a starting position...')
        [start_x, start_y] = select_area()
        split_colors(color_palette, converted_image, start_x, start_y, brush_size=BRUSH_SIZE, bias=BIAS)
    except Exception as e:
        print(e)
