%% milestone 3 test 
f = @(x) ((x-5)^2 +5); 
xf = 1:0.5:5;
resultf =  zeros(9,2);
for i =1:length(xf)
    resultf(i,1) = xf(i);
    resultf(i,2) = f(xf(i));
end

figure()
stem(resultf(:,1),resultf(:,2))
grid on

g = @(x) (-(x+5)^2 -5);
xg = -9:0.5:-1;
resultg = zeros(17,2);
for i =1:length(xg)
    resultg(i,1) = (20/8)*xg(i);
    resultg(i,2) = (20/16)*g(xg(i));
end

figure()
stem(resultg(:,1),resultg(:,2))
grid on 
