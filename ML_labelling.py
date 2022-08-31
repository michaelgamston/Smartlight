"""
Tkinter GUI for efficient image labelling
ToDo:
- Adapt for base64 encoding
"""

import tkinter as tk
from tkinter import ttk
import matplotlib

matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import os
import numpy as np
import boto3
from PIL import Image
from io import BytesIO


s3 = boto3.resource(
    service_name="s3",
    region_name="eu-west-2",
    aws_access_key_id="AKIAZ252ONHLAUQMV6U2",
    aws_secret_access_key="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",
)

DUMP_PATH = "~/Desktop/smartlight/s3_jpegs/"


class Widget:
    def __init__(self, H=None, W=None, bucketname="imagestestsmartlight", folder="test_images/"):
        """
        Change bucketname and folder path in constructor
        Also can change H and W to reflect a change in image resolution if needed
        """
        self.bucket = s3.Bucket(bucketname)
        self.folder = folder
        self.fig = Figure(figsize=(9, 6), dpi=150)
        self.fig.tight_layout()
        self.root = tk.Tk()
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.root)
        self.canvas.get_tk_widget().grid(column=0, row=3)
        self.files = [
            obj
            for obj in self.bucket.objects.all()
            if obj.key[: len(self.folder)] == self.folder
            and len(obj.key) > len(self.folder)
        ]
        self.file_idx = 0
        self.arr = None
        self.H = H
        self.W = W
        self.im = None
        get_image_button = tk.Button(
            self.root, text="Next image", command=self.get_image_binary
        )
        get_image_button.grid(row=1, column=1)
        # label images and upload as npy to corresponding s3 folder
        class1_button = tk.Button(
            self.root, text="class 1", command=lambda: self.label("1")
        )
        class1_button.grid(row=1, column=2)
        class2_button = tk.Button(
            self.root, text="class 2", command=lambda: self.label("2")
        )
        class2_button.grid(row=1, column=3)
        save_jpeg_button = tk.Button(self.root, text="save jpeg", command=self.save_jpeg)
        save_jpeg_button.grid(row=2, column=1)
        self.get_image_binary()
        tk.mainloop()

    def get_image_binary(self):
        """
        This function works for binary files in S3 bucket. 
        Assumes image is contained within first H*W bytes. 
        Will need to change approach if working with base64 encoded image in JSON file
        """
        with open("tmp_file", "r+b") as data:
            obj = self.files[self.file_idx % len(self.files)]
            # download binary file into data variable
            self.bucket.download_fileobj(obj.key, data)
            # reset cursor to start of file
            data.seek(0)
            # binary to numpy array
            self.arr = np.frombuffer(
                data.read(self.H * self.W), dtype=np.uint8
            ).reshape((self.H, self.W))
        self.display_image()
        self.file_idx += 1

    # ToDo
    def get_image_b64(self):
        ...
    
    
    def display_image(self):
        self.fig.clf()
        self.sp = self.fig.add_subplot(111)
        self.im = self.sp.imshow(self.arr, cmap="binary_r", vmin=0, vmax=255)
        self.sp.tick_params(
            axis="both",
            which="both",
            bottom=False,
            top=False,
            left=False,
            right=False,
            labelleft=False,
            labelbottom=False,
        )
        self.canvas.draw()
    
    def save_jpeg(self):
        im = Image.fromarray(self.arr, mode="L")
        filename = self.files[self.file_idx].key[len(self.folder) :]
        full_save_path = os.path.expanduser(DUMP_PATH + filename + ".jpeg")
        im.save(full_save_path)

    def label(self, Class):
        name = self.files[self.file_idx].key[len(self.folder):]
        key = f"class_{Class}/{name}"
        bytes_ = BytesIO()
        np.save(bytes_, self.arr, allow_pickle=True)
        bytes_.seek(0)
        self.bucket.upload_fileobj(bytes_, key)
 

if __name__ == "__main__":
    w = Widget(H=75, W=100)
