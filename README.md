# Panoramic
Realtime panorama stitching

How to:

1)Install mongoose-cpp

2)Install mjpg-streamer

Running:
1) Place your plugins for streaming into stream folder:
 - input_file.so
 - output_http.so
2) `cmake .`
3) `make -j 2`
4) For streaming type: `mjpg_streamer -i  "/root/data/Panoramic/plugins/input_file.so -f /root/data/Panoramic/img -n result.jpg" -o "/root/data/Panoramic/plugins/output_http.so -w ./www"`