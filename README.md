PUT_AndroidOrientation
======================

The EKF-based quaternion orientation estimation for Android devices. The repository contains free simple examples estimating the orientation of the Android smartphone:
- implementation in NDK using Eigen library (OrientEstimation_EKF_EIGEN)
- implementation in NDK using OpenCV library (OrientEstimation_EKF_OpenCV)
- implementation of AKEF (!) in NDK using Eigen library (OrientEstimation_AEKF_EIGEN) [preferred]

The code is published under the MiT license, so feel free to use it for your own purposes.

More information about the implemented methods can be found in (please share and cite if you found this code useful):
J. Gośliński, M. Nowicki, P. Skrzypczyński, "Performance Comparison of EKF-based Algorithms for Orientation Estimation on Android Platform", IEEE Sensors, Journal

When having problems running the software , found some bug or anything relevant concerning the method or code, please contact:
michal.nowicki (at) put.poznan.pl

======================

Please expect a open indoor localization library for Android combining all mayor algorithms fused together:
- WiFi fingerprinting
- Magnetic distortions
- Orientation estimation
- Stepometer
- Vision-based motion estimation
- Barometer 
- ...
More soon ...