ffmpeg -i ballpit.mp4 -r 30 -vf scale=512:-1 \
  -ss 00:00:03 -to 00:00:10 -f image2pipe -vcodec ppm - |
  convert -deconstruct -delay 5 -loop 0 - ballpit.gif
