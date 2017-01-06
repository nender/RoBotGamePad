# RoBotGamePad (rbgp) and RobotGamePad\_Arduino (rbgp_\a)

This is the control software I wrote for a robotics class back in the day. It allowed us to use a gamepad on a laptop to drive the arduino-based robot (the chassis was based on an electric go-kart) The design was quite simple.
One process ran on a laptop, which was on the same local network as the robot. Using SDL to capture gamepad events, I dumped the entire state of the gamepad (which was quite small) into a buffer, and sent it via UDP to the arduino.
The arduino then unpacked this data and used it to signal the motor controllers.

Obviously this is inefficient and assumes really good reliability of network, but it was the simplest possible thing that could work, and work it did.