```markdown created by ChatGPT```
# Skribbl.io AutoDraw Bot

This project is an autodraw bot that can draw anything based on a text prompt, specifically designed to play with Skribbl.io (an online word drawing guessing game). The bot uses Google Custom Search to find images based on your prompt, processes these images, and then draws them on Skribbl.io.

## Project Structure

```
.
├── palette/
│   ├── color_palette.png
│   ├── palette.json
├── .env
├── input.png
├── main.py
├── requirements.txt
├── screen_capture.py
```

- **palette/color_palette.png**: The image of the color palette used by Skribbl.io.
- **palette/palette.json**: The JSON file containing color palette data.
- **.env**: Environment file containing the Google Custom Search API key.
- **input.png**: Placeholder for the input image.
- **main.py**: Main script containing the core logic.
- **requirements.txt**: List of required packages.
- **screen_capture.py**: Script containing the `ScreenCapture` class for selecting a screen area.

## Prerequisites

- Python 3.7+
- Google Custom Search API Key

## Installation

1. Clone the repository:

```sh
git clone https://github.com/raz0229/skribblio-autodraw-bot.git
cd skribblio-autodraw-bot
```

2. Create a virtual environment and activate it:

```sh
python -m venv venv
source venv/bin/activate  # On Windows, use `venv\Scripts\activate`
```

3. Install the required packages:

```sh
pip install -r requirements.txt
```

4. Create a `.env` file in the project root directory and add your Google Custom Search API key:

```
GOOGLE_CUSTOM_SEARCH_DEVELOPER_KEY=your_api_key_here
```

## Usage

1. Run the main script:

```sh
python main.py
```

2. A prompt will appear asking for an image search term. Enter your desired term.

3. The script will download an image based on the search term, scale it, convert it to the custom color palette used by Skribbl.io, and prompt you to select a starting position on the Skribbl.io canvas.

4. The bot will then draw the image on the canvas.

## Custom Color Palette

The color palette used by the bot is defined in `palette/palette.json`. This file contains the RGB values of the colors used by Skribbl.io and their positions on the palette image.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request for any improvements or bug fixes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [Skribbl.io](https://skribbl.io) for the drawing game.
- [Google Custom Search API](https://developers.google.com/custom-search) for image search functionality.
- [Python](https://www.python.org) and its community for providing excellent libraries and support.

