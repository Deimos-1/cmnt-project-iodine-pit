function T = dualnewtonsolver(dX, dI, I0, X0, t0, dt, tf)

    Niter = floor((tf - t0)/dt) + 1;
    I = I0*ones(1,Niter); 
    X = X0*ones(1,Niter); 


    for i = 2:1:Niter
        I(i) = I(i-1) + dt*dI(I(i-1));
        X(i) = X(i-1) + dt*dX(I(i-1), X(i-1));
    end

    T = table(t0:dt:tf, X,'VariableNames',{'t' 'X'});
end