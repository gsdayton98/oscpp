// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 8/11/23.
//
#include <algorithm>
#include "circular_buffer.hpp"


// For convenience we instantiate instances of some common types.
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<bool>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<unsigned char>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<unsigned short>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<unsigned int>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<unsigned long>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<unsigned long long>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<char>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<short>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<int>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<long>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<long long>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<float>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<double>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<long double>;
