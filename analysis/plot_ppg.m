% -----------------------------------------------------------------------------
% This should be paired with process_ppg.m
%
% The numbers indicate which section of code in process_ppg.m 
% should be executed before plotting
% -----------------------------------------------------------------------------

set(0,'defaulttextinterpreter','latex')

% 2.
N = length(s_o);
t = linspace(0, N*Ts, N);
[x0, y0, width, height] = deal(0, 0, 8, 2.5);

figure('Units','inches','Position', [x0 y0 width height],'PaperPositionMode','auto');
p = plot(t, s_o, 'LineWidth', 0.75);
xlabel('Time (sec)'); ylabel('Normalized Amplitude')
axis([20, 30, min(s_o)-0.01, max(s_o)+0.01])
set(gca, 'FontSize', 12)

print('./images/normalized_IR', '-depsc')


%-------------------------------------
% 3. calculate the frequency response 
%-------------------------------------
[h, f]  = freqz(b, a, N,  Fs);

figure('Units','inches','Position', [x0 y0 width height],'PaperPositionMode','auto');
p = plot(f, abs(h), ...
        [fc_low, fc_low], [0,1], 'r--',...
        [fc_high, fc_high], [0,1], 'r--');

p(1).LineWidth = 1.5;
grid;
xlabel('Frequency (Hz)'); ylabel('$|H(f)|$')
axis([-0.5, 15, -0.01, 1.09]);
set(gca, 'FontSize', 12)
print('./images/freq_response', '-depsc')

%-------------------------------------
% 6. Post HR calculations
%-------------------------------------
N = length(ds);
t = linspace(0, N*Ts, N);
thresh_line = repmat(thresh, 1, length(t));

ix = ixs(pks > thresh);

figure('Units','inches','Position', [x0 y0 width height+1],'PaperPositionMode','auto');
p = plot(t, ds, ...
        t, thresh_line, 'k--', ...
        t(ix), ds(ix), 'ro');

p(1).LineWidth = 0.75;
p(2).LineWidth = 0.75;
xlabel('Time (sec)')
axis([20, 30, min(ds)-0.01, 1.25])
set(gca, 'FontSize', 12)
chH = get(gca,'Children');
set(gca,'Children',flipud(chH))
legend({ 'peak','threshold','$\frac{d}{dt}x(t)$'}, 'Interpreter', 'LaTex')

print('./images/derivative_thresh_IR', '-depsc')


% ------------------------------------

% 2.
[x0, y0, width, height] = deal(0, 0, 9, 6);
fig = figure('Units','inches',...
            'Position', [x0 y0 width height],...
            'PaperPositionMode','auto');

subplot(2,1,1)
N = length(s_o);
t = linspace(0, N*Ts, N);
p = plot(t, s_o, 'LineWidth', 0.75);
axis([20, 30, min(s_o)-0.01, max(s_o)+0.01])
set(gca, 'FontSize', 12)
legend({'$x(t)$'}, 'Interpreter', 'LaTex')

subplot(2,1,2)
N = length(ds);
t = linspace(0, N*Ts, N);

p = plot(t, ds, ...
        t, thresh_line, 'k--', ...
        t(ix), ds(ix), 'ro');
p(1).LineWidth = 0.75; p(2).LineWidth = 0.75;
xlabel('Time (sec)')
axis([20, 30, min(ds)-0.01, 1.25])
set(gca, 'FontSize', 12)
chH = get(gca,'Children');
set(gca,'Children',flipud(chH))
legend({ 'peak','threshold','$\frac{d}{dt}x(t)$'}, ...
        'Interpreter', 'LaTex', 'Orientation', 'horizontal')

h = axes('Position',[0 0 1 1],'Visible','off'); %add an axes on the left side of your subplots
set(gcf,'CurrentAxes',h)
text(0.08,.4,'Normalized Amplitude',...
'VerticalAlignment','bottom',...
'HorizontalAlignment','left', 'Rotation', 90, 'FontSize',12)

print('./images/raw_and_dxdt', '-depsc')
