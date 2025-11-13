function T = newtonsolver(f, y0, t0, dt, tf)

    Niter = floor((tf - t0)/dt) + 1;
    Y = y0*ones(1,Niter);

    for i = 2:1:Niter
        Y(i) = Y(i-1) + dt*f(Y(i-1));
    end
    T = table(t0:dt:tf, Y,'VariableNames',{'t' 'y'});
end