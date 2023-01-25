"""
generate python module for KalmanFilter, ClarkeTransform, and ParkTransform classes
"""
import cppyy
cppyy.cppdef(kalman_filter_code)
cppyy.cppdef(clarke_transform_code)
cppyy.cppdef(park_transform_code)

from cppyy.gbl import KalmanFilter, ClarkeTransform, ParkTransform

kalman_filter = KalmanFilter(3, 3, 0)
x = Matrix(3, 3, 0)
P = Matrix(3, 3, 0)
F = Matrix(3, 3, 0)
Q = Matrix(3, 3, 0)
H = Matrix(3, 3, 0)
R = Matrix(3, 3, 0)
u = Matrix(3, 3, 0)
x_ = Matrix(3, 3, 0)
P_ = Matrix(3, 3, 0)
kalman_filter(x, P, F, Q, H, R, u, x_, P_)

clarke_transform = ClarkeTransform(3, 3, 0)
x = Matrix(3, 3, 0)
P = Matrix(3, 3, 0)
F = Matrix(3, 3, 0)
Q = Matrix(3, 3, 0)
H = Matrix(3, 3, 0)
R = Matrix(3, 3, 0)
u = Matrix(3, 3, 0)
x_ = Matrix(3, 3, 0)
P_ = Matrix(3, 3, 0)
clarke_transform(x, P, F, Q, H, R, u, x_, P_)

park_transform = ParkTransform(3, 3, 0)
x = Matrix(3, 3, 0)
P = Matrix(3, 3, 0)
F = Matrix(3, 3, 0)
Q = Matrix(3, 3, 0)
H = Matrix(3, 3, 0)
R = Matrix(3, 3, 0)
u = Matrix(3, 3, 0)
x_ = Matrix(3, 3, 0)
P_ = Matrix(3, 3, 0)
park_transform(x, P, F, Q, H, R, u, x_, P_)
