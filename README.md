# Base Streamer
Uses ZED Depth Camera to access depth frames to then display it for operator

## Notes:
- Before running command, user can set MIN_DIST and MAX_DIST for ZED Camera in FT.
If not specified, initializes as MIN_DIST = 1, MAX_DIST = 20
ie. ./zed_depth 1 20
- ZED Camera contains multiple depth mode:
    -> ULTRA: offers the highest depth range and better preservers Z-accuracy along the sensing range
    -> QUALITY: has a stronger filtering stage giving smoother surfaces
    -> PERFORMANCE: designed to be smooth, can miss some details
