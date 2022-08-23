"""
At present images are uploaded to AWS S3 container as strings of binary. 
This script converts binary into a numpy array and saves as a JPEG.

"""

import binascii
import numpy as np
import matplotlib.pyplot as plt
import os
import argparse
import timeit
from time import process_time_ns
from PIL import Image

# change this to whichever directory your images are downloaded to
LOAD_PATH = "~/Documents/AWS/s3_images/"
SAVE_PATH = "~/Documents/AWS/s3_images/converted/"  # change this to whichever directory you want to dump images


def main(file=None, H=None, W=None, show=False):
    full_load_path = os.path.expanduser(LOAD_PATH + file)
    # open binary file and read into hex string
    with open(full_load_path, "rb") as f:
        s = binascii.hexlify(f.read())
    # Hex string to 2d np array
    im_arr = np.array([int(s[i : i + 2], 16) for i in range(0, W * H * 2, 2)]).reshape(
        (H, W)
    )
    im = Image.fromarray(im_arr.astype(np.uint8), mode="L")
    # Optionally display image
    if show:
        plt.imshow(im_arr, cmap="binary_r", vmin=0, vmax=255)
        plt.show()
    full_save_path = os.path.expanduser(SAVE_PATH + file + ".jpeg")
    im.save(full_save_path)


def check_positive(val):
    """
    Validate image dimension
    """
    error_message = f"{val} is invalid argument, must be a positive integer"
    try:
        ival = int(val)
        if ival <= 0:
            raise argparse.ArgumentTypeError(error_message)
    except ValueError:
        raise argparse.ArgumentTypeError(error_message)
    return ival


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--file",
        "-n",
        help="path to binary file",
        type=str,
        default="1660899791341",
    )
    parser.add_argument("-H", help="Image height dim", type=check_positive)
    parser.add_argument("-W", help="Image width dimension", type=check_positive)
    args = parser.parse_args()
    main(**vars(args))
