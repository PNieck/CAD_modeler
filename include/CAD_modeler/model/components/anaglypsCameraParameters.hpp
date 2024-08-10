#pragma once

#include "position.hpp"
#include "cameraParameters.hpp"


class AnaglyphsCameraParameters: public CameraParameters {
public:
    AnaglyphsCameraParameters(const CameraParameters& camParams, float eyeSeparation, float convergence):
        CameraParameters(camParams), eyeSeparation(eyeSeparation), convergence(convergence) {}

    float eyeSeparation;
    float convergence;
};
