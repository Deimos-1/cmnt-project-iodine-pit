function dI_dt = dI_dt(iodineFissionYield, fissionSection, neutronFlux, iodineHalfLife_hr, iodine)
    iodineDecayConst = utils.decayConst(iodineHalfLife_hr);
    dI_dt = iodineFissionYield*fissionSection*neutronFlux - iodineDecayConst*iodine;
end