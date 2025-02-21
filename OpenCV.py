# Import necessary libraries
import cv2
import matplotlib.pyplot as plt
import cvlib as cv
import urllib.request
import numpy as np
from cvlib.object_detection import draw_bbox
import concurrent.futures
import subprocess

# https://pjreddie.com/darknet/yolo/ 
# URL of the camera feed
camera_url = 'http://10.148.24.190/cam-hi.jpg'
image = None

# Function to display live transmission from the camera
def display_live_transmission():
    # Create a window for live transmission
    cv2.namedWindow("live transmission", cv2.WINDOW_AUTOSIZE)
    while True:
        # Get the image from the camera URL
        image_response = urllib.request.urlopen(camera_url)
        image_np = np.array(bytearray(image_response.read()), dtype=np.uint8)
        image = cv2.imdecode(image_np, -1)

        # Show the live transmission
        cv2.imshow('live transmission', image)
        key = cv2.waitKey(5)
        # Break the loop if 'q' is pressed
        if key == ord('q'):
            break
            
    # Destroy all windows after the loop
    cv2.destroyAllWindows()
    
# Function to display object detection on the camera feed
def display_object_detection():
    # Create a window for object detection
    cv2.namedWindow("detection", cv2.WINDOW_AUTOSIZE)
    while True:
        try:
            # Get the image from the camera URL
            image_response = urllib.request.urlopen(camera_url)
            image_np = np.array(bytearray(image_response.read()), dtype=np.uint8)
            image = cv2.imdecode(image_np, -1)

            # Detect common objects in the image
            bbox, label, conf = cv.detect_common_objects(image)
            # Draw bounding box around detected objects
            image = draw_bbox(image, bbox, label, conf)

            # If an object is detected, send a curl command with the object name to the database
            for obj in label:
                cmd = f"curl -X POST -d 'column1={obj}&column2=detected' your_database_url"
                process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
                process.wait()
                if process.returncode == 0:
                    print(f'{obj} data sent successfully.')
                else:
                    print('Failed to send object data.')
                    subprocess.run(["python", "UpdateP.py"])
                    print("[Object Detection Updated.")

            # Show the object detection
            cv2.imshow('detection', image)
        except Exception as e:
            print(f"Error in object detection: {e}")
        
        key = cv2.waitKey(5)
        # Break the loop if 'q' is pressed
        if key == ord('q'):
            break
            
    # Destroy all windows after the loop
    cv2.destroyAllWindows()


# Main function
if __name__ == '__main__':
    print("started")
    # Use ProcessPoolExecutor to run both functions concurrently
    with concurrent.futures.ProcessPoolExecutor() as executor:
        live_transmission_future = executor.submit(display_live_transmission)
        object_detection_future = executor.submit(display_object_detection)
