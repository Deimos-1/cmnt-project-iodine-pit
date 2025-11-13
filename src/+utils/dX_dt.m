function dX_dt = dX_dt(xenonFissionYield, fissionSection, neutronFlux, iodineHalfLife_hr, iodine, xenonHalfLife_hr, xenon, xenonAbsobtionSection)
    iodineDecayConst = decayConst(iodineHalfLife_hr);
    xenonDecayConst = decayConst(xenonHalfLife_hr) ;
    dX_dt = xenonFissionYield*fissionSection*neutronFlux + iodineDecayConst*iodine - xenonDecayConst*xenon - xenonAbsobtionSection*xenon*neutronFlux;
end