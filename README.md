# RC-Buggy-Project

this is really three variations of the same project. Worked on with Triona McNamara, Adam Peat and Heather Mitchell.

### drive_by_direction version
- This version forgoes the sensors and uses the inbuilt imu to work out the direction. It uses a Madgwick filter from hte Madgwick library to work out coordinates with reasnoble accuracy.

### pde_object_following
- This version follows a black line using two IR sensors to go around a track. It also can follow and object in front of it at a target distance fo 20cm using a PDE controller
