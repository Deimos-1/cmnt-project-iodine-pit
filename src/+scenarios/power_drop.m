
function power_drop(data)

    dI = @(I, flux) utils.dI_dt(...
            data.fissionYield.I135, ...
            data.macroFissionSection, ...
            flux, ...
            data.halfLife_hr.I135, ...
            I...
        );

    dX = @(I, X, flux) utils.dX_dt( ...
            data.fissionYield.Xe135, ...
            data.macroFissionSection, ...
            flux, ...
            data.halfLife_hr.I135, ...
            I, ...
            data.halfLife_hr.Xe135, ...
            X, ...
            data.absobtionSection.Xe135...
        );

    % coupled system to feed into ode15s
    % strucutre given by ChatGPT 
    function dy = coupled_iod_xe(t, y, flux)
        I = y(1); X = y(2);
        delta_I = dI(I, flux);
        delta_X = dX(I, X, flux);
        dy = [delta_I; delta_X];
    end
    
    % analytical equilibrium concentration
    I0 = data.fissionYield.I135*data.macroFissionSection*data.nominalNeutronFlux ...
        /utils.decayConst(data.halfLife_hr.I135) ;
    assert(dI(I0, data.nominalNeutronFlux) == 0)

    % equilibrium concentration
    Xeq = fzero(@(Xe) dX(I0, Xe, data.nominalNeutronFlux), 1e20);
    fprintf("Xeq = %0.2f \n", Xeq);
    fprintf("Ieq = %0.2f \n", I0);
    assert(dX(I0, Xeq, data.nominalNeutronFlux) == 0)

    % timespan
    t0 = 0;
    tf = 100*3600; % 100 hr should be enough
    tspan = [0, tf];

    
    %% Looping through power drop scenarios
    for i = 0:0.1:1 % all 10's % of the neutron flux
        
        % initial conditions
        flux = data.nominalNeutronFlux*i;
        y0 = [I0; Xeq];            
        opts = odeset('RelTol',1e-6,'AbsTol',1e-9);
        
        % cols: t, I, X
        [TT, YY] = ode15s(@(t,y) coupled_iod_xe(t,y,flux), tspan, y0, opts);
        
        % Results from the MATLAB implementation of RK4 are matching
        % [TT_RK4, II_RK4, XX_RK4] = utils.RK4solver(...
        %     @(I) dI(I, flux), ...
        %     @(I, X) dX(I, X, flux), ...
        %     I0, ...
        %     Xeq, ...
        %     tspan, ...
        %     10 ...
        % );

        hold on
        plot(TT/3600, 1e5*(data.absobtionSection.Xe135/(2.3*data.macroFissionSection))*YY(:,2)); xlabel('t [hr]'); ylabel('- \rho_{Xe} [pcm]');
        % 2.3 is the number of neutrons released per fission 
        % formula from Zechuan D., Solving Bateman Equation for Xenon
        % Transient Analysis Using Numerical Methods (2018)
        % plot(TT_RK4/3600, (data.absobtionSection.Xe135/(2.3*data.macroFissionSection))*XX_RK4, LineStyle="-."); 
        hold on
        
        
        
    end
    % Results from the C implementation are matching
    Cres = readtable("../results/results.csv"); 
    plot(Cres.t / 3600, 1e5*(data.absobtionSection.Xe135/(2.3*data.macroFissionSection))*Cres.X, LineStyle='-.', LineWidth=1.5, Color='black');
    grid on
    grid minor

end

