
module.exports = process.env.SOPHIST_COV
  ? require('./lib-cov/sophist')
  : require('./lib/sophist');
