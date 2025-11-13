function I = I(iodineFissionYield, fissionSection, neutronFlux, iodineHalfLife_hr, I0, t)
    iodineDecayConst = utils.decayConst(iodineHalfLife_hr);
    I = iodineFissionYield*fissionSection*neutronFlux/iodineDecayConst ...
        + (I0 - iodineFissionYield*fissionSection*neutronFlux/iodineDecayConst)*exp(-iodineDecayConst*t);
end