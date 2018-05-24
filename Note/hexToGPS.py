#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu May 24 22:15:52 2018

@author: lsangild
"""
# Input
long = 0x285a1406;
lat  = 0xb8dd7a21;

# Flib bytes
long = 0x06145a28;
lat  = 0x217addb8;

# Flip to big endian
long = long*10**(-7);
lat = lat*10**(-7);