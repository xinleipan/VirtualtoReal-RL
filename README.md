## Virtual to Real Reinforcement Learning for Autonomous Driving ##
#### In BMVC 2017 (Spotlight Presentation) ####
By Xinlei Pan*, Yurong You*, Ziyan Wang, Cewu Lu

## Introduction

Virtual to real reinforcement learning proposed a realistic translation
network that can preprocess virtual images from simulator and translate
to their realistic counterparts. It has been proved by experiments that 
our method can train a driving policy entirely in virtual environments that
can adapt to real world driving data. 

Virtual to real reinforcement learning for autonomous driving was initially
described in an [arXiv technical report](https://arxiv.org/abs/1704.03952).

This repo contains a [Torch](http://torch.ch/) implementation of our virtual
to real reinforcement learning framework. The code contains two main parts:
TORCS car racing simulator and image translation network.

## License

## Citing Virtual to Real Reinforcement Learning for Autonomous Driving

If you find this work or our implementation useful in your research, 
please cite:

    @article{you2017virtual,
        title={Virtual to Real Reinforcement Learning for Autonomous Driving},
        author={You, Yurong and Pan, Xinlei and Wang, Ziyan and Lu, Cewu},
        journal={arXiv preprint arXiv:1704.03952},
        year={2017}
    }

## Installation of TORCS simulation environments

The TORCS car racing simulator can be installed to collect 
images, their segmentations. It can also be installed as a
reinforcement learning training environment. 

For detailed instructions, see [here](https://github.com/YurongYou/rlTORCS)
for more. 
### Install TORCS to collect virtual images
Follow the directions in 
[README.md](https://github.com/xinleipan/VirtualtoReal-RL/blob/master/TORCS/README.md) 
to install TORCS. 

### Install TORCS to collect virtual images' segmentation
Replace folder `TORCS/torcs-1.3.6/data/tracks/e-track-1` 
with `data/TORCS/e-track-1`; replace folder `TORCS/torcs-1.3.6/
data/tracks/road/g-track-2` with `data/TORCS/g-track-2`. 
Remove previous installed TORCS and reinstall
TORCS following [README.md](https://github.com/xinleipan/VirtualtoReal-RL/blob/master/TORCS/README.md). 

### Training virtual-to-seg and seg-to-real networks
Example pictures of virtual-to-seg and seg-to-real 
translation has been put in data/examples/seg-to-real.png.

To train segmentation to realistic image translation
network, download the segmented data from [here](https://drive.google.com/file/d/0B6QMkcB4kQntOUx5SzJSWUc5ZlU/view?usp=sharing). Then follow pix2pix network
to train the seg-to-real network.
 
