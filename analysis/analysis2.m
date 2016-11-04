set(0,'defaulttextinterpreter','latex')

% -----------------------------
%   Load IR data recording
% -----------------------------
x = [];

filename = './data/recording_20161029_190929.txt';
fid = fopen(filename);
tline = fgetl(fid);
while ischar(tline)
    x = [x, str2num(tline)];
    tline = fgetl(fid);
end
fclose(fid);

% -----------------------------------
%   Look at raw signal 
% -----------------------------------
Fs = 125;
Ts = 1/Fs;
N = length(x);
t = linspace(0, N*Ts, N);

x_o = x;
t_o = t;

%x = x_o; t = t_o;

% Remove the DC offset and normalize
x = x - mean(x);
x = x/max(abs(x));


p = plot(t, x);
p(1).LineWidth = 1;
xlabel('Time (sec)')
ylabel('Normalized Amplitude')
axis([20, 30, min(x)-0.01, max(x)+0.01])
set(gca, 'FontSize', 13)




% -------------------------------------------------
%           Band-Pass Butterworth Filter
% -------------------------------------------------
order    = 2;
fc_low  = 0.4;
fc_high  = 5;
W = [fc_low, fc_high] * (Fs/2)^-1 ;


[b,a] = butter(order, [fc_low,fc_high]/(Fs/2), 'bandpass');

% calculate the frequency response 
[h, f]  = freqz(b, a, N,  Fs);

% ---------------------------------
clf
p = plot(f, abs(h), ...
        [fc_low, fc_low], [0,1], 'r--',...
        [fc_high, fc_high], [0,1], 'r--');

p(1).LineWidth = 1.5;
grid;
xlabel('Frequency (Hz)'); ylabel('$|H(f)|$')
axis([-0.5, 15, -0.01, 1.09]);
set(gca, 'FontSize', 14)
% ---------------------------------
k = 10;
m = movmean(x, k);
%s = filter(b,a,x);
s = filter(b,a,m);


% remove transients from filter by chopping of first and last 0.75 sec
ts = t((t > min(t) + 0.75) & (t < max(t) - 0.75));
s = s((t > min(t) + 0.75) & (t < max(t) - 0.75));

% Calculate derivative
ds = zeros(1, length(s) - 1);
for k = 2:length(s)
    ds(k-1) = (s(k) - s(k - 1)); 
end

ds = ds/max(abs(ds));



thresh = 0.025;
thresh_line = repmat(thresh, 1, length(ts));
p = plot(ts, [ds, 0], ts, thresh_line, 'r--', ts, [ds, 0], 'ro');
axis([20, 30, min(ds)-0.01, max(ds)+0.01])
xlabel('Time (sec)')
set(gca, 'FontSize', 13)
p(1).LineWidth = 0.75;
p(2).LineWidth = 0.75;


beat_detected = 0;
beats = 0;
[pks, ixs] = findpeaks(ds);
plot(ts, [ds, 0], ts(ixs(pks > thresh)), ds(ixs(pks > thresh)), 'ro')

beats = sum(pks > thresh);
((beats/2)/max(ts))*60






thresh_line = repmat(thresh, 1, length(ts));

subplot(2,1,1)
p = plot(ts, s)
axis([20, 30, min(s)-0.01, max(s)+0.01])

subplot(2,1,2)
p = plot(ts, [ds_dt, 0], ts, thresh_line, 'r--');
title('DC offset Removed and Normalization');
xlabel('Time (sec)')
axis([20, 30, min(ds_dt)-0.01, max(ds_dt)+0.01])
set(gca, 'FontSize', 14)



