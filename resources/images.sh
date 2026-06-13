#!/bin/bash
DEST_DIR=~/git/pebble_uqm_pilot_face/resources/images
HD4X_DIR=~/Downloads/hires4x/hires4x/ships/
BASE_DIR=~/Downloads/base/ships
SHIPS="avatar
avenger
blade
broodhome
cruiser
dreadnought
drone
eluder
fury
guardian
intruder
jugger
marauder
mauler
nemesis
penetrator
podship
probe
scout
skiff
stinger
terminator
torch
trader
xform"

do_convert() {
  postfix=$1
  convert $FILE -define png:preserve-colormap=true -alpha set /tmp/temp.png
  outfile=${ship}${postfix}

  rm "${DEST_DIR}/${outfile}~color.png"
  ffmpeg -i /tmp/temp.png -vf \
"scale=144:-2:flags=lanczos,\
unsharp=7:7:1.0:3:3:0.5,\
eq=contrast=1.2,\
split[s0][s1];\
[s0]palettegen=max_colors=64[p];\
[s1][p]paletteuse=dither=floyd_steinberg" \
  ${DEST_DIR}/${outfile}~color.png

  rm "${DEST_DIR}/${outfile}~bw.png"
    convert "${DEST_DIR}/${outfile}~color.png" \
    -colorspace Gray \
    -filter Triangle \
    -sigmoidal-contrast 6x50% \
    -dither FloydSteinberg \
    -remap pattern:gray50 \
    "${DEST_DIR}/${outfile}~bw.png"

  #p2
  rm ${DEST_DIR}/${outfile}~color~200w.png
  ffmpeg -i /tmp/temp.png -vf \
"scale=200:-2:flags=lanczos,\
unsharp=7:7:1.0:3:3:0.5,\
eq=contrast=1.2,\
split[s0][s1];\
[s0]palettegen=max_colors=64[p];\
[s1][p]paletteuse=dither=floyd_steinberg" \
  ${DEST_DIR}/${outfile}~color~200w.png
  #round2
  rm ${DEST_DIR}/${outfile}~color~260w.png
  ffmpeg -i /tmp/temp.png -vf \
"scale=260:-2:flags=lanczos,\
unsharp=7:7:1.0:3:3:0.5,\
eq=contrast=1.2,\
split[s0][s1];\
[s0]palettegen=max_colors=64[p];\
[s1][p]paletteuse=dither=floyd_steinberg" \
  ${DEST_DIR}/${outfile}~color~260w.png
  rm /tmp/temp.png
}

for ship in ${SHIPS}; do
  FILE=$(find $HD4X_DIR -name $ship-cap-000.png | head -n1)
  if [[ ! -f $FILE ]]; then echo "no $ship"; exit 1; fi
  do_convert _hires
  FILE=$(find $BASE_DIR -name $ship-cap-000.png | head -n1)
  if [[ ! -f $FILE ]]; then echo "no $ship"; exit 1; fi
  do_convert _hires
done
