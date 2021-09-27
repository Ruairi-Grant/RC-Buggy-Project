# RC-Buggy-Project

This is really two variations of the same project. It is a buggy controlled by an arduino nano 33 iot, and a processing GUI.  
Worked on with **Triona McNamara, Adam Peat and Heather Mitchell**.

### pde_object_following
- This version follows a black line using two IR sensors to go around a track. It also can follow and object in front of it at a target distance fo 20cm using a PDE controller

### drive_by_direction
- This version forgoes the sensors and uses the inbuilt imu to work out the direction. It uses a Madgwick filter from hte Madgwick library to work out coordinates with reasnoble accuracy.


