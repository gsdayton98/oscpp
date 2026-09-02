// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023. Glen S. Dayton. Rights reserved according to included license.
//
// Created by Glen Dayton on 7/17/23.
//

#include "random_device.hpp"

auto oscpp::RandomDevice::entropy() const -> double { return r.entropy(); }
