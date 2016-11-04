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

clf
subplot(2,1,1)
plot(t_o, x_o)
title('Raw PPG Signal')
axis([20, 30, min(x_o)-0.01, max(x_o)+0.01])
set(gca, 'FontSize', 14)

subplot(2,1,2)
plot(t, x, 'r')
title('DC offset Removed and Normalization');
xlabel('Time (sec)')
axis([20, 30, min(x)-0.01, max(x)+0.01])
set(gca, 'FontSize', 14)


ax = gca;
outerpos = ax.OuterPosition;
ti = ax.TightInset; 
left = outerpos(1) + ti(1) ;
bottom = outerpos(2) + ti(2);
ax_width = outerpos(3) - (ti(1) + ti(3));
ax_height = outerpos(4) - (ti(2) + ti(4));
ax.Position = [left, bottom, ax_width, ax_height];







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

title('Frequency Response of Band-Pass Filter');
xlabel('Frequency (Hz)')
ylabel('$|H(f)|$')
axis([-0.5, 15, -0.01, 1.09]);
set(gca, 'FontSize', 14)


%------------------
% Apply the filter
%------------------
k = 20;
m = movmean(x, k);

%s = filter(b,a,x);
s = filter(b,a,m);


% remove transients from filter by chopping of first and last 0.75 sec
ts = t((t > min(t) + 0.75) & (t < max(t) - 0.75));
s = s((t > min(t) + 0.75) & (t < max(t) - 0.75));
plot(ts,s)

s = s/max(abs(s));
plot(ts, s)


% Calculate derivative
ds_dt = zeros(1, length(s) - 1);
for k = 2:length(s)
    ds_dt(k-1) = (s(k) - s(k - 1)); % leave off (Ts)^-1 for efficiency
end


thresh_line = repmat(thresh, 1, length(ts));
p = plot(ts, [ds_dt, 0], ts, thresh_line, 'r--');

thresh = 0.02;
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

beats
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



%p.Marker = 'o';
%p.MarkerEdgeColor = 'r';












