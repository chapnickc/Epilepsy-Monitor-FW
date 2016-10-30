%run('~/Google Drive/Fall2016/LinearSystems/code/lib/init.m')
set(0,'defaulttextinterpreter','latex')

% -----------------------------------
% read in the IR data recording
% -----------------------------------
x = [];

filename = './recording_20161029_190929.txt';
fid = fopen(filename);
tline = fgetl(fid);
while ischar(tline)
    x = [x, str2num(tline)];
    tline = fgetl(fid);
end
fclose(fid);

% -----------------------------------
% Look at raw signal 
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


subplot(2,1,1)
plot(t_o, x_o)
title('Raw PPG Signal')

subplot(2,1,2)
plot(t, x, 'r')
title('DC offset Removed and Normalization');
xlabel('Time (sec)')


% -------------------------------------------------
%           Apply band-pass butterworth filter
% -------------------------------------------------

order    = 4;
fcutlow  = 0.4;
fcuthigh = 5;
[b,a]    = butter(order,[fcutlow,fcuthigh]/(Fs/2), 'bandpass');

[h, f]  = freqz(b, a, N,  Fs);
plot(f, abs(h))
title('Frequency Response of Digital Band-pass Filter');
xlabel('Frequency (Hz)')
axis([-0.5, 15, -0.1, 1.1]);


% Apply the filter
s = filter(b,a,x);

% remove transients from filter by chopping of first and last 0.75 sec
ts = t((t > min(t) + 0.75) & (t < max(t) - 0.75));
s = s((t > min(t) + 0.75) & (t < max(t) - 0.75));
plot(ts,s)


ds_dt = zeros(1, length(s) - 1);

for k = 2:length(s)
    ds_dt(k-1) = (s(k) - s(k - 1))/(Ts);
end

p = plot(ts, [ds_dt, 0]);
p.Marker = 'o';
p.MarkerEdgeColor = 'r';


thresh = 4;
beat_detected = 0;
beats = 0;

for sample = ds_dt
    if sample > thresh & ~(beat_detected)
        beat_detected = 1;
        beats = beats + 1;
    elseif (sample < thresh) & beat_detected
        beat_detected = 0;
    end
end

((beats/2)/max(t))*60
