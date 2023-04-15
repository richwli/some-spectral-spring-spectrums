# importing packages
from pytube import YouTube
from pydub import AudioSegment
import os
import subprocess
  
# url input from user
link = "https://youtu.be/tOlmVIRJaAU"
destination = r"."
yt = YouTube(link)
  
# extract only audio
video = yt.streams.filter(only_audio=True).first()
  
# check for destination to save file
# print("Enter the destination (leave blank for current directory)")
# destination = '.'
  
# download the file
out_file = video.download(output_path=destination)
print("Done downloading")
# save the file
base, ext = os.path.splitext(out_file)
new_file = base + '.mp3'
os.rename(out_file, new_file)
  
# result of success
print(yt.title + " has been successfully downloaded.")
#sound = AudioSegment.from_mp3(new_file)
#sound.export(destination, format="wav")
audio_file = AudioSegment.from_file(new_file)
audio_file.export(f"{yt.title}.wav", "wav")