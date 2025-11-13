function decayConst = decayConst(halfLife_hr)
    decayConst = log(2)/(halfLife_hr*60*60); % given in [/s]
    % https://www.nuclear-power.com/nuclear-power/reactor-physics/atomic-nuclear-physics/radioactive-decay/radioactive-decay-law/decay-constant/
end