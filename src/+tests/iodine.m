function iodine(neutronFlux, data)
    
    %% initialization 
    Ieq = data.fissionYield.I135*data.macroFissionSection*neutronFlux/utils.decayConst(data.halfLife_hr.I135) ;
    I0 = Ieq/2;
    tf = 48*60*60; % [s]
    dt = 1;        % [s]

    % functions
    dI = @(I) utils.dI_dt(data.fissionYield.I135, data.macroFissionSection, neutronFlux, data.halfLife_hr.I135, I);
    I_t = @(t, I0) utils.I(data.fissionYield.I135, data.macroFissionSection, neutronFlux, data.halfLife_hr.I135, I0, t);

    %% solving
    T = utils.eulersolver(dI, I0, 0, dt, tf);

    %% plotting the results
    hold on
    plot(T.t, T.y, LineWidth=1.5)
    plot(T.t, I_t(T.t, I0), LineStyle='--',LineWidth=2)
    line(T.t, Ieq*ones(size(T.t)), Color='red')
    text(5, 1.01*Ieq, "Equilibrium concentration",Color='red')
    xlabel("t [s]")
    ylabel("concentration [#/cm^3]")
    title("Iodine 135 concentration evolution")
    legend('num', 'exact')

end