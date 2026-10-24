"""Synthetic regression: grain must not become geometry; broad volume survives."""
from pathlib import Path
import sys
import numpy as np
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from depth import shape_grid
y,x=np.mgrid[-1:1:256j,-1:1:256j]
mask=np.ones(x.shape,bool)
body=0.3+0.5*np.exp(-(x*x+y*y)*3)
noise=np.random.default_rng(42).normal(0,0.12,x.shape).astype(np.float32)
a=shape_grid(body,mask).astype(float)
b=shape_grid(body+noise,mask).astype(float)
c=shape_grid(0.5+noise,mask).astype(float)
assert np.ptp(a)>65,np.ptp(a)
assert np.mean(np.abs(a-b))<3,np.mean(np.abs(a-b))
assert np.max(np.abs(c))<8,np.max(np.abs(c))
assert not np.any(a[0]) and not np.any(a[-1]) and not np.any(a[:,0]) and not np.any(a[:,-1])
print('PASS broad depth survives; noisy input remains stable; flat grain suppressed; rims pinned')
