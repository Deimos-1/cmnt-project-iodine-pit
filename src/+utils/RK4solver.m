function [TT, II, XX] = RK4solver(dI, dX, I0, X0, tspan, dt)
    % 4th order Runge-Kutta solver, with no time dependency.
    % tested against ode15s and was very accurate (dt=10 [s]).
    assert(numel(tspan)==2, "Invalid tspan, should be of the form [t0 tf] but got: %f\n", tspan)
    
    % initial conditions 
    TT = tspan(1):dt:tspan(2);
    II = I0*ones(size(TT));
    XX = X0*ones(size(TT));

    for i = 2:length(TT)

        % start with Iodine: 
        k1 = dI(II(i-1));           % 1st order Taylor approximation
        I1 = II(i-1) + k1*dt/2;     % midpoint from 1st approximation
        k2 = dI(I1);                % slope at the midpoint
        I2 = II(i-1) + k2*dt/2;     % better estimate
        k3 = dI(I2); 
        I3 = II(i-1) + k3*dt/2;
        k4 = dI(I3);
        
        II(i) = II(i-1) + (k1 + 2*k2 + 2*k3 + k4)*dt/6;

        % same for Xenon: 
        % I did not know what values of I to pass to estimate dX, 
        % according to ChatGPT, it should be I, I1, etc.
        k1 = dX(II(i-1), XX(i-1));
        X1 = XX(i-1) + k1*dt/2;    
        k2 = dX(I1, X1);                
        X2 = XX(i-1) + k2*dt/2;     
        k3 = dX(I2, X2); 
        X3 = XX(i-1) + k3*dt/2;
        k4 = dX(I3, X3);
        
        XX(i) = XX(i-1) + (k1 + 2*k2 + 2*k3 + k4)*dt/6;
    end
end