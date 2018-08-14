import os 
os.environ['TF_CPP_MIN_LOG_LEVEL']='0'
import pbcvt
import numpy as np
import time
import cv2
from PIL import Image

'''password = "nvidia"
command = "sudo sh -c 'echo 2048 > /sys/module/usbcore/parameters/usbfs_memory_mb'"
os.system("echo %s|sudo -S %s" % (password, command))
'''
#Initiation of load mapping and flycapture
print("[...] Warming up camera ")
pbcvt.Init_Flycapture()
pbcvt.Init_LoadMapping()
	


frame = 1
while True:
	try:	
		start = time.time()
		input_image = pbcvt.Stream_Flycapture()
		height,width,_ = input_image.shape
		print(input_image.shape)
		print("Frame: {}".format(frame))
		input_image = cv2.resize(input_image,(1224, 820))
		cv2.imshow("FLYCAP",input_image)
		cv2.waitKey(30)
		#print("HERE")
		#left_view = input_image[480:600, 0:1024]
		#left_view = cv2.resize(left_view, (1024, 240))
		#right_view = input_image[120:240, 0:1024]
		#right_view = cv2.resize(right_view, (1024, ))
		
		#cv2.imshow("DEPTH_WINDOW_LEFT",left_view)
		#cv2.waitKey(30)
		#cv2.imshow("DEPTH_WINDOW_RIGHT",right_view)
		#cv2.waitKey(30)
		frame = frame+1
		print(input_image.shape)
		print("time in Seconds :" + str(1/(time.time()-start)))
		print("************************************************")
	except(TypeError,AttributeError,KeyboardInterrupt):
		break	


