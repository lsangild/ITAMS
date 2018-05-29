#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu May 24 22:15:52 2018

@author: lsangild
"""
# Input
long = 0x285a1406;
lat  = 0xb8dd7a21;

# Flip bytes
long = 0x061327f8;
lat  = 0x217b356f;

# Scale
long = long*10**(-7);
lat = lat*10**(-7);

print(long);
print(lat);