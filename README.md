# RC-Buggy-Project

This is really two variations of the same project. It is a buggy controlled by an arduino nano 33 iot, and a processing GUI. The .fzz file is a sketch of the circuit involved, created with fritzing.    
Worked on with **Triona McNamara, Adam Peat and Heather Mitchell**.

### pde_object_following
- This version follows a black line using two IR sensors to go around a track. It also can follow and object in front of it at a target distance fo 20cm using a PDE controller

### drive_by_direction
- This version forgoes the sensors and uses the built-in imu to work out the direction. It uses a Madgwick filter from the Madgwick library to work out coordinates with reasonable accuracy.


