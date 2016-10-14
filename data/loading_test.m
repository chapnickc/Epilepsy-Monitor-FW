run('~/Google Drive/Fall2016/LinearSystems/code/lib/init.m')

x = [];

filename = './recording_20160910_231614.txt';
fid = fopen(filename);

tline = fgetl(fid);
while ischar(tline)
    x = [x, str2num(tline)];
    tline = fgetl(fid);
end
fclose(fid);

Ts = 0.050;
N = length(x);
t = linspace(0, N*Ts, N);

plot(t, x)

ix = find(t > 80 & t < 100);
plot(t(ix), x(ix))

[z, th, h] = myfilter(t, x, [5], 2);
[f, Hf] = myFT(th, h);
plot(f, Hf)
plot(t, z)
